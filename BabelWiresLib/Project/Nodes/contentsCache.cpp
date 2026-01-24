/**
 * The ContentsCache summarizes the contents of a Node visible to the user.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Nodes/contentsCache.hpp>

#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/Project/Nodes/editTree.hpp>
#include <BabelWiresLib/TypeSystem/type.hpp>
#include <BabelWiresLib/Types/Generic/genericType.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableType.hpp>
#include <BabelWiresLib/ValueNames/valueNames.hpp>
#include <BabelWiresLib/ValueTree/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/ValueTree/valueTreeGenericTypeUtils.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

#include <unordered_set>

babelwires::ContentsCacheEntry::ContentsCacheEntry(std::string label, const ValueTreeNode* input,
                                                   const ValueTreeNode* output, const Path& path, std::uint8_t depth,
                                                   std::uint8_t indent)
    : m_label(std::move(label))
    , m_input(input)
    , m_output(output)
    , m_path(path)
    , m_depth(depth)
    , m_indent(indent)
    , m_isExpandable(false)
    , m_isExpanded(false)
    , m_hasModifier(false)
    , m_isStructureEditable(true)
    , m_hasFailedModifier(false)
    , m_hasHiddenModifiers(false)
    , m_hasFailedHiddenModifiers(false)
    , m_hasSubModifiers(false)
    , m_hasUnassignedInputTypeVariable(false)
    , m_hasUnassignedOutputTypeVariable(false) {}

bool babelwires::ContentsCacheEntry::isOrHasUnassignedInputTypeVariable() const {
    return m_hasUnassignedInputTypeVariable || (m_input && m_input->getType()->tryAs<babelwires::TypeVariableType>());
}

bool babelwires::ContentsCacheEntry::isOrHasUnassignedOutputTypeVariable() const {
    return m_hasUnassignedOutputTypeVariable || (m_output && m_output->getType()->tryAs<babelwires::TypeVariableType>());
}

babelwires::ContentsCache::ContentsCache(EditTree& edits)
    : m_edits(edits) {}

namespace babelwires {
    namespace Detail {

        struct ContentsCacheBuilder {
            ContentsCacheBuilder(std::vector<ContentsCacheEntry>& rows, EditTree& edits)
                : m_identifierRegistry(IdentifierRegistry::read())
                , m_rows(rows)
                , m_edits(edits) {}

            IdentifierRegistry::ReadAccess m_identifierRegistry;
            std::vector<ContentsCacheEntry>& m_rows;
            /// Stack of generic types encountered when exploring the tree, as indices into m_rows
            std::vector<unsigned int> m_inputGenericTypeStack;
            /// Stack of generic types encountered when exploring the tree, as indices into m_rows
            std::vector<unsigned int> m_outputGenericTypeStack;
            EditTree& m_edits;

            /// Sets some compound details and returns whether the compound is expanded or not.
            /// indentInOut will be set to the indent level of children.
            bool setAndGetCompoundIsExpanded(const babelwires::ValueTreeNode* compound, const Path& path,
                                             bool hasChildren, std::uint8_t& indentInOut) {
                assert(compound);
                ++indentInOut;
                ContentsCacheEntry& row = m_rows.back();
                row.m_isExpandable = hasChildren;
                if (hasChildren) {
                    row.m_isExpanded = m_edits.isExpanded(path);
                    return row.m_isExpanded;
                }
                return false;
            }

            struct GenericTypeInfo {
                int m_depthInUnassignedGenericTree = -1;
            };

            template <bool isInput>
            void markRowsWithUnassignedTypeVariables(unsigned int currentRowIndex, unsigned int levelsFromEnd) {
                const unsigned int genericTypeStackSize =
                    (isInput ? m_inputGenericTypeStack : m_outputGenericTypeStack).size();
                if (levelsFromEnd >= genericTypeStackSize) {
                    levelsFromEnd = genericTypeStackSize - 1;
                }
                const unsigned int genericTypeRowIndex =
                    (isInput ? m_inputGenericTypeStack
                             : m_outputGenericTypeStack)[genericTypeStackSize - 1 - levelsFromEnd];
                while (currentRowIndex > genericTypeRowIndex) {
                    if constexpr (isInput) {
                        m_rows[currentRowIndex].m_hasUnassignedInputTypeVariable = true;
                    } else {
                        m_rows[currentRowIndex].m_hasUnassignedOutputTypeVariable = true;
                    }
                    currentRowIndex = getParentRowIndex(currentRowIndex);
                }
            }

            /// This does two jobs:
            /// 1. If the feature is a GenericType that has an unassigned type variable, then set the provided boolean
            /// argument.
            ///    - That signals to the traversal algorithm that some of the subtrees below this will have an
            ///    unassigned type variables.
            /// 2. If the feature is a TypeVariable, then mark all the rows between this and the corresponding
            /// GenericType as having an unassigned type variable.
            template <bool isInput>
            void handleGenericTypes(const ValueTreeNode* valueTreeNode, unsigned int numChildren,
                                    bool isExpanded, GenericTypeInfo& genericTypeInfo) {
                if (genericTypeInfo.m_depthInUnassignedGenericTree >= 0) {
                    if (numChildren == TypeVariableType::c_numChildren) {
                        if (valueTreeNode->getType()->tryAs<TypeVariableType>()) {
                            const auto typeVarData =
                                TypeVariableData::isTypeVariable(valueTreeNode->getTypeExp());
                            assert(typeVarData);
                            unsigned int currentRowIndex = getParentRowIndex(m_rows.size() - 1);
                            markRowsWithUnassignedTypeVariables<isInput>(currentRowIndex,
                                                                         typeVarData->m_numGenericTypeLevels);
                        }
                    } else if (!isExpanded) {
                        // In the case of a collapsed node, we have to traverse the valueTreeNode now, since the cache
                        // algorithm stops exploring at collapsed nodes.
                        ContentsCacheEntry& thisEntry = m_rows.back();
                        const int heightOfUnassignedTypeVariable = getMaximumHeightOfUnassignedGenericType(
                            *valueTreeNode, genericTypeInfo.m_depthInUnassignedGenericTree);
                        if (heightOfUnassignedTypeVariable >= 0) {
                            unsigned int currentRowIndex = m_rows.size() - 1;
                            markRowsWithUnassignedTypeVariables<isInput>(currentRowIndex,
                                                                         heightOfUnassignedTypeVariable);
                        }
                    }
                }
                if (numChildren == GenericType::c_numChildren) {
                    // If this is a generic type, increase the depth.
                    // We deliberately perform after testing for collapsed nodes, since the generic type itself
                    // shouldn't be treated as a level when querying its own getMaximumHeightOfUnassignedGenericType.
                    if (const GenericType* type = valueTreeNode->getType()->tryAs<GenericType>()) {
                        // Record the row regardless of whether there are unassigned type variables or not.
                        (isInput ? m_inputGenericTypeStack : m_outputGenericTypeStack).push_back(m_rows.size() - 1);
                        // increment the depth if this generic type or any above it had unassigned type variables.
                        if (type->isAnyTypeVariableUnassigned(valueTreeNode->getValue()) ||
                            (genericTypeInfo.m_depthInUnassignedGenericTree >= 0)) {
                            ++genericTypeInfo.m_depthInUnassignedGenericTree;
                        }
                    }
                }
            }

            void handleGenericInputTypes(const babelwires::ValueTreeNode* valueTreeNode, unsigned int numChildren,
                                         bool isExpanded, GenericTypeInfo& genericTypeInfo) {
                handleGenericTypes<true>(valueTreeNode, numChildren, isExpanded, genericTypeInfo);
            }

            void handleGenericOutputTypes(const babelwires::ValueTreeNode* valueTreeNode, unsigned int numChildren,
                                          bool isExpanded, GenericTypeInfo& genericTypeInfo) {
                handleGenericTypes<false>(valueTreeNode, numChildren, isExpanded, genericTypeInfo);
            }

            void addInputFeatureToCache(std::string label, const babelwires::ValueTreeNode* valueTreeNode,
                                        const Path& path, std::uint8_t depth, std::uint8_t indent,
                                        GenericTypeInfo genericTypeInfo) {
                ContentsCacheEntry& thisRow = m_rows.emplace_back(
                    ContentsCacheEntry(std::move(label), valueTreeNode, nullptr, path, depth, indent));
                const unsigned int numChildren = valueTreeNode->getNumChildren();
                const bool isExpanded = setAndGetCompoundIsExpanded(valueTreeNode, path, numChildren, indent);
                handleGenericInputTypes(valueTreeNode, numChildren, isExpanded, genericTypeInfo);
                if (isExpanded) {
                    ++depth;
                    for (int i = 0; i < valueTreeNode->getNumChildren(); ++i) {
                        const ValueTreeNode* child = valueTreeNode->getChild(i);
                        // TODO Needless cost doing this.
                        PathStep step = valueTreeNode->getStepToChild(child);
                        Path pathToChild = path;
                        pathToChild.pushStep(step);
                        std::ostringstream os;
                        step.writeToStreamReadable(os, *m_identifierRegistry);
                        addInputFeatureToCache(os.str(), child, std::move(pathToChild), depth, indent, genericTypeInfo);
                    }
                }
            }

            void addOutputFeatureToCache(std::string label, const babelwires::ValueTreeNode* valueTreeNode,
                                         const Path& path, std::uint8_t depth, std::uint8_t indent,
                                         GenericTypeInfo genericTypeInfo) {
                ContentsCacheEntry& thisRow = m_rows.emplace_back(
                    ContentsCacheEntry(std::move(label), nullptr, valueTreeNode, path, depth, indent));
                const unsigned int numChildren = valueTreeNode->getNumChildren();
                const bool isExpanded =
                    setAndGetCompoundIsExpanded(valueTreeNode, path, valueTreeNode->getNumChildren(), indent);
                handleGenericOutputTypes(valueTreeNode, numChildren, isExpanded, genericTypeInfo);
                if (isExpanded) {
                    ++depth;
                    for (int i = 0; i < numChildren; ++i) {
                        const ValueTreeNode* child = valueTreeNode->getChild(i);
                        // TODO Needless cost doing this.
                        PathStep step = valueTreeNode->getStepToChild(child);
                        Path pathToChild = path;
                        pathToChild.pushStep(step);
                        std::ostringstream os;
                        step.writeToStreamReadable(os, *m_identifierRegistry);
                        addOutputFeatureToCache(os.str(), child, std::move(pathToChild), depth, indent,
                                                genericTypeInfo);
                    }
                }
            }

            void addFeatureToCache(std::string label, const ValueTreeNode* input, const ValueTreeNode* output,
                                   const Path& path, std::uint8_t depth, std::uint8_t indent,
                                   GenericTypeInfo inputDepthInUnassignedGenericTree,
                                   GenericTypeInfo outputDepthInUnassignedGenericTree) {
                ContentsCacheEntry& thisRow =
                    m_rows.emplace_back(ContentsCacheEntry(std::move(label), input, output, path, depth, indent));
                const unsigned int numInputChildren = input->getNumChildren();
                const unsigned int numOutputChildren = output->getNumChildren();
                // Assume expandability is common to input and output feature.
                const bool isExpanded =
                    setAndGetCompoundIsExpanded(input, path, numInputChildren + numOutputChildren, indent);
                handleGenericInputTypes(input, numInputChildren, isExpanded, inputDepthInUnassignedGenericTree);
                handleGenericOutputTypes(output, numOutputChildren, isExpanded, outputDepthInUnassignedGenericTree);
                if (isExpanded) {
                    ++depth;
                    std::unordered_set<int> outputIndicesHandled;
                    for (int i = 0; i < input->getNumChildren(); ++i) {
                        const ValueTreeNode* child = input->getChild(i);
                        // TODO Needless cost doing this.
                        PathStep step = input->getStepToChild(child);
                        Path pathToChild = path;
                        const int outputChildIndex = output->getChildIndexFromStep(step);
                        pathToChild.pushStep(step);
                        std::ostringstream os;
                        step.writeToStreamReadable(os, *m_identifierRegistry);
                        if (outputChildIndex >= 0) {
                            addFeatureToCache(os.str(), input->getChild(i), output->getChild(outputChildIndex),
                                              std::move(pathToChild), depth, indent, inputDepthInUnassignedGenericTree,
                                              outputDepthInUnassignedGenericTree);
                            outputIndicesHandled.insert(outputChildIndex);
                        } else {
                            addInputFeatureToCache(os.str(), input->getChild(i), std::move(pathToChild), depth, indent,
                                                   inputDepthInUnassignedGenericTree);
                        }
                    }
                    for (int i = 0; i < output->getNumChildren(); ++i) {
                        if (outputIndicesHandled.find(i) == outputIndicesHandled.end()) {
                            const ValueTreeNode* child = output->getChild(i);
                            // TODO Needless cost doing this.
                            PathStep step = output->getStepToChild(child);
                            Path pathToChild = path;
                            pathToChild.pushStep(step);
                            std::ostringstream os;
                            step.writeToStreamReadable(os, *m_identifierRegistry);
                            addOutputFeatureToCache(os.str(), child, std::move(pathToChild), depth, indent,
                                                    outputDepthInUnassignedGenericTree);
                        }
                    }
                }
            }

            /// Get the parent row index of the given row index, using depth.
            unsigned int getParentRowIndex(unsigned int rowIndex) {
                assert(rowIndex > 0);
                assert(rowIndex < m_rows.size());
                assert(m_rows[0].getDepth() == 0);
                assert(m_rows[rowIndex].getDepth() > 0);
                const unsigned int depth = m_rows[rowIndex].getDepth();
                do {
                    --rowIndex;
                } while (m_rows[rowIndex].getDepth() >= depth);
                return rowIndex;
            }
        };

    } // namespace Detail
} // namespace babelwires

void babelwires::ContentsCache::setValueTrees(std::string rootLabel, const ValueTreeNode* input,
                                              const ValueTreeNode* output) {
    m_rows.clear();
    Detail::ContentsCacheBuilder builder(m_rows, m_edits);
    if (input && output) {
        builder.addFeatureToCache(std::move(rootLabel), input, output, Path(), 0, 0,
                                  Detail::ContentsCacheBuilder::GenericTypeInfo(),
                                  Detail::ContentsCacheBuilder::GenericTypeInfo());
    } else if (input) {
        builder.addInputFeatureToCache(std::move(rootLabel), input, Path(), 0, 0,
                                       Detail::ContentsCacheBuilder::GenericTypeInfo());
    } else if (output) {
        builder.addOutputFeatureToCache(std::move(rootLabel), output, Path(), 0, 0,
                                        Detail::ContentsCacheBuilder::GenericTypeInfo());
    } else {
        assert(!"Invalid case");
    }
    setChanged(Changes::StructureChanged);
    updateModifierFlags();
}

void babelwires::ContentsCache::updateModifierCache() {
    updateModifierFlags();
}

void babelwires::ContentsCache::updateModifierFlags() {
    if (!m_rows[0].getInput()) {
        /// Only input features can be modified.
        return;
    }

    using Iterator = EditTree::Iterator<EditTree>;

    struct StackData {
        StackData(int index)
            : m_index(index) {}

        /// The editTree iterators of the children of this row.
        std::unordered_map<PathStep, Iterator> m_iteratorFromStep;

        int m_index;

        bool m_hasSubModifiers = false;
    };
    std::vector<StackData> stackDataStack;

    stackDataStack.emplace_back(0);
    const static unsigned int notADepth = std::numeric_limits<unsigned int>::max();
    unsigned int depthOfLastConnectionModifier = notADepth;

    // The edit iterator which matches the current row.
    // It's set to m_edit.end() if there aren't any edits at or below this row.
    auto editIt = m_edits.begin();

    for (int index = 0; index < m_rows.size(); ++index) {
        if (index > 0) {
            // See if there is an appropriate editTree iterator in the parent's set.
            stackDataStack.emplace_back(index);
            const PathStep& stepToHere = m_rows[index].m_path.getLastStep();
            StackData& parentStackData = stackDataStack[stackDataStack.size() - 2];
            const auto it = parentStackData.m_iteratorFromStep.find(stepToHere);
            if (it != parentStackData.m_iteratorFromStep.end()) {
                editIt = it->second;
                parentStackData.m_iteratorFromStep.erase(it);
            } else {
                editIt = m_edits.end();
            }
        }

        // Use the edit iterator for this row if there is one.
        {
            ContentsCacheEntry& currentRow = m_rows[index];

            if (depthOfLastConnectionModifier < currentRow.m_depth) {
                currentRow.m_isStructureEditable = false;
            } else {
                depthOfLastConnectionModifier = notADepth;
                currentRow.m_isStructureEditable = true;
            }

            if (editIt != m_edits.end()) {
                if (const Modifier* modifier = editIt.getModifier()) {
                    currentRow.m_hasModifier = true;
                    currentRow.m_hasFailedModifier = modifier->isFailed();
                    if ((depthOfLastConnectionModifier == notADepth) && modifier->asConnectionModifier()) {
                        depthOfLastConnectionModifier = currentRow.m_depth;
                        currentRow.m_isStructureEditable = false;
                    }
                } else {
                    currentRow.m_hasModifier = false;
                    currentRow.m_hasFailedModifier = false;
                }
            } else {
                currentRow.m_hasModifier = false;
                currentRow.m_hasFailedModifier = false;
            }
        }

        // Find the set of edits for children at this row.
        if (editIt != m_edits.end()) {
            StackData& stackData = stackDataStack.back();
            for (auto childIt = editIt.childrenBegin(); childIt != editIt.childrenEnd(); childIt.nextSibling()) {
                stackData.m_iteratorFromStep.insert(std::make_pair(childIt.getStep(), childIt));
            }
        }

        // Pop finished items off the stack.
        // Note that this index may represent the last entry for ancestor modifiers.
        {
            const int numStepsNextIndex = (index < m_rows.size() - 1) ? m_rows[index + 1].m_path.getNumSteps() : 0;
            while (!stackDataStack.empty() &&
                   (numStepsNextIndex <= m_rows[stackDataStack.back().m_index].m_path.getNumSteps())) {
                auto& lastStackData = stackDataStack.back();
                auto& lastRow = m_rows[lastStackData.m_index];
                lastRow.m_hasHiddenModifiers = false;
                lastRow.m_hasFailedHiddenModifiers = false;
                // We've traversed all the children of the lastStackData, so
                // review whether it remains in charge of some modifiers.
                for (auto [_, it] : lastStackData.m_iteratorFromStep) {
                    if (const Modifier* modifier = it.getModifier()) {
                        lastRow.m_hasHiddenModifiers = true;
                        if (modifier->isFailed()) {
                            lastRow.m_hasFailedHiddenModifiers = true;
                            break;
                        }
                    }
                }
                const bool anyModifiers =
                    lastStackData.m_hasSubModifiers || lastRow.m_hasModifier || lastRow.m_hasHiddenModifiers;
                lastRow.m_hasSubModifiers = anyModifiers;
                stackDataStack.pop_back();
                // Record in the parent if there are submodifiers in this child.
                if (anyModifiers && !stackDataStack.empty()) {
                    stackDataStack.back().m_hasSubModifiers = true;
                }
            }
        }
    }
}

const babelwires::ContentsCacheEntry* babelwires::ContentsCache::getEntry(int i) const {
    if ((0 <= i) && (i < m_rows.size())) {
        return &m_rows[i];
    }
    // TODO This is defensive. Is this necessary?
    return nullptr;
}

int babelwires::ContentsCache::getNumRows() const {
    return m_rows.size();
}

int babelwires::ContentsCache::getIndexOfPath(bool seekInputFeature, const Path& path) const {
    for (int i = 0; i < m_rows.size(); ++i) {
        const ContentsCacheEntry& entry = m_rows[i];
        if ((seekInputFeature && !entry.m_input) || (!seekInputFeature && !entry.m_output)) {
            continue;
        }
        if (path == m_rows[i].m_path) {
            return i;
        }
    }
    return -1;
}

bool babelwires::ContentsCache::isChanged(Changes changes) const {
    return (m_changes & changes) != Changes::NothingChanged;
}

void babelwires::ContentsCache::clearChanges() {
    m_changes = Changes::NothingChanged;
}

void babelwires::ContentsCache::setChanged(Changes changes) {
    m_changes = m_changes | changes;
}
