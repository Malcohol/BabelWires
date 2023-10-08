/**
 * The ContentsCache summarizes the contents of a FeatureElement visible to the user.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/contentsCache.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Features/compoundFeature.hpp>
#include <BabelWiresLib/FileFormat/fileFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/editTree.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/ValueNames/valueNames.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <unordered_set>

babelwires::ContentsCacheEntry::ContentsCacheEntry(std::string label, const Feature* inputFeature,
                                                   const Feature* outputFeature, const FeaturePath& path,
                                                   std::uint8_t indent)
    : m_label(std::move(label))
    , m_inputFeature(inputFeature)
    , m_outputFeature(outputFeature)
    , m_path(path)
    , m_indent(indent)
    , m_isExpandable(false)
    , m_isExpanded(false)
    , m_hasModifier(false)
    , m_hasLocalModifier(false)
    , m_hasFailedModifier(false)
    , m_hasHiddenModifiers(false)
    , m_hasFailedHiddenModifiers(false)
    , m_hasSubModifiers(false) {}

babelwires::ContentsCache::ContentsCache(EditTree& edits)
    : m_edits(edits) {}

namespace {
    std::string getArrayEntryLabel(int i, const babelwires::ValueNames* names) {
        std::ostringstream os;
        os << "[" << i << "]";
        std::string name;
        if (names && names->getNameForValue(i, name)) {
            os << " (" << name << ")";
        }
        return os.str();
    }
} // namespace

namespace babelwires {
    namespace Detail {

        struct ContentsCacheBuilder {
            ContentsCacheBuilder(std::vector<ContentsCacheEntry>& rows, EditTree& edits)
                : m_identifierRegistry(IdentifierRegistry::read())
                , m_rows(rows)
                , m_edits(edits) {}

            /// Sets some compound details and returns whether the compound is expanded or not.
            /// indentInOut will be set to the indent level of children.
            bool setAndGetCompoundIsExpanded(const babelwires::CompoundFeature* compound, const FeaturePath& path,
                                             bool hasChildren, std::uint8_t& indentInOut) {
                assert(compound);
                const Feature::Style style = compound->getStyle();
                if (isZero(style & babelwires::Feature::Style::isInlined)) {
                    ++indentInOut;
                }
                ContentsCacheEntry& row = m_rows.back();
                if (isNonzero(style & babelwires::Feature::Style::isCollapsable)) {
                    row.m_isExpandable = hasChildren;
                    if (hasChildren) {
                        row.m_isExpanded = m_edits.isExpanded(path);
                        return row.m_isExpanded;
                    }
                } else {
                    if (path.getNumSteps() > 0) {
                        m_edits.setImplicitlyExpanded(path, true);
                    }
                    row.m_isExpandable = false;
                }
                return true;
            }

            void addInputFeatureToCache(std::string label, const babelwires::Feature* f, const FeaturePath& path,
                                        std::uint8_t indent) {
                m_rows.emplace_back(ContentsCacheEntry(std::move(label), f, nullptr, path, indent));
                if (const auto* compound = f->as<const babelwires::CompoundFeature>()) {
                    addInputCompoundFeatureToCache(compound, path, indent);
                }
            }

            void addInputCompoundFeatureToCache(const babelwires::CompoundFeature* compound, const FeaturePath& path,
                                                std::uint8_t indent) {
                if (setAndGetCompoundIsExpanded(compound, path, compound->getNumFeatures(), indent)) {
                    for (int i = 0; i < compound->getNumFeatures(); ++i) {
                        const Feature* child = compound->getFeature(i);
                        // TODO Needless cost doing this.
                        PathStep step = compound->getStepToChild(child);
                        FeaturePath pathToChild = path;
                        pathToChild.pushStep(step);
                        std::ostringstream os;
                        step.writeToStreamReadable(os, *m_identifierRegistry);
                        addInputFeatureToCache(os.str(), child, std::move(pathToChild), indent);
                    }
                }
            }

            void addOutputFeatureToCache(std::string label, const babelwires::Feature* f, const FeaturePath& path,
                                         std::uint8_t indent) {
                m_rows.emplace_back(ContentsCacheEntry(std::move(label), nullptr, f, path, indent));
                if (const auto* compound = f->as<const babelwires::CompoundFeature>()) {
                    addOutputCompoundFeatureToCache(compound, path, indent);
                }
            }

            void addOutputCompoundFeatureToCache(const babelwires::CompoundFeature* compound, const FeaturePath& path,
                                                 std::uint8_t indent) {
                if (setAndGetCompoundIsExpanded(compound, path, compound->getNumFeatures(), indent)) {
                    for (int i = 0; i < compound->getNumFeatures(); ++i) {
                        const Feature* child = compound->getFeature(i);
                        // TODO Needless cost doing this.
                        PathStep step = compound->getStepToChild(child);
                        FeaturePath pathToChild = path;
                        pathToChild.pushStep(step);
                        std::ostringstream os;
                        step.writeToStreamReadable(os, *m_identifierRegistry);
                        addOutputFeatureToCache(os.str(), child, std::move(pathToChild), indent);
                    }
                }
            }

            void addFeatureToCache(std::string label, const Feature* inputFeature, const Feature* outputFeature,
                                   const FeaturePath& path, std::uint8_t indent) {
                m_rows.emplace_back(ContentsCacheEntry(std::move(label), inputFeature, outputFeature, path, indent));
                if (const auto* const inputCompound = inputFeature->as<const babelwires::CompoundFeature>()) {
                    const auto* const outputCompound = outputFeature->as<const babelwires::CompoundFeature>();
                    assert(outputCompound && "If the input is a record, the output must be a record too");
                    addCompoundContentsToCache(inputCompound, outputCompound, path, indent);
                }
            }

            void addCompoundContentsToCache(const CompoundFeature* inputFeature, const CompoundFeature* outputFeature,
                                            const FeaturePath& path, std::uint8_t indent) {
                // Assume expandability is common to input and output feature.
                if (setAndGetCompoundIsExpanded(
                        inputFeature, path, inputFeature->getNumFeatures() + outputFeature->getNumFeatures(), indent)) {
                    std::unordered_set<int> outputIndicesHandled;
                    for (int i = 0; i < inputFeature->getNumFeatures(); ++i) {
                        const Feature* child = inputFeature->getFeature(i);
                        // TODO Needless cost doing this.
                        PathStep step = inputFeature->getStepToChild(child);
                        FeaturePath pathToChild = path;
                        const int outputChildIndex = outputFeature->getChildIndexFromStep(step);
                        pathToChild.pushStep(PathStep(step));
                        std::ostringstream os;
                        step.writeToStreamReadable(os, *m_identifierRegistry);
                        if (outputChildIndex >= 0) {
                            addFeatureToCache(os.str(), inputFeature->getFeature(i),
                                              outputFeature->getFeature(outputChildIndex), std::move(pathToChild),
                                              indent);
                            outputIndicesHandled.insert(outputChildIndex);
                        } else {
                            addInputFeatureToCache(os.str(), inputFeature->getFeature(i), std::move(pathToChild),
                                                   indent);
                        }
                    }
                    for (int i = 0; i < outputFeature->getNumFeatures(); ++i) {
                        if (outputIndicesHandled.find(i) == outputIndicesHandled.end()) {
                            const Feature* child = outputFeature->getFeature(i);
                            // TODO Needless cost doing this.
                            PathStep step = outputFeature->getStepToChild(child);
                            FeaturePath pathToChild = path;
                            pathToChild.pushStep(step);
                            std::ostringstream os;
                            step.writeToStreamReadable(os, *m_identifierRegistry);
                            addOutputFeatureToCache(os.str(), child, std::move(pathToChild), indent);
                        }
                    }
                }
            }

            IdentifierRegistry::ReadAccess m_identifierRegistry;
            std::vector<ContentsCacheEntry>& m_rows;
            EditTree& m_edits;
        };

    } // namespace Detail
} // namespace babelwires

void babelwires::ContentsCache::setFeatures(const RootFeature* inputFeature, const RootFeature* outputFeature) {
    m_rows.clear();
    Detail::ContentsCacheBuilder builder(m_rows, m_edits);
    const babelwires::Feature* const rootFeature = inputFeature ? inputFeature : outputFeature;
    // TODO Instead of this hack, make the extra "file" row a UI feature.
    const char* rootLabel = rootFeature->as<const babelwires::FileFeature>() ? "File" : "Root";
    if (inputFeature && outputFeature) {
        builder.addFeatureToCache(rootLabel, inputFeature, outputFeature, FeaturePath(), 0);
    } else if (inputFeature) {
        builder.addInputFeatureToCache(rootLabel, inputFeature, FeaturePath(), 0);
    } else if (outputFeature) {
        builder.addOutputFeatureToCache(rootLabel, outputFeature, FeaturePath(), 0);
    } else {
        assert(!"Unimplemented");
    }
    setChanged(Changes::StructureChanged);
    updateModifierFlags();
    setIndexOffset();
}

void babelwires::ContentsCache::updateModifierCache() {
    updateModifierFlags();
    setIndexOffset();
}

void babelwires::ContentsCache::setIndexOffset() {
    const int oldIndexOffset = m_indexOffset;
    const ContentsCacheEntry& rootEntry = m_rows[0];
    const babelwires::Feature* const rootFeature = rootEntry.getInputThenOutputFeature();
    m_indexOffset = (rootEntry.hasFailedHiddenModifiers() || rootFeature->as<const babelwires::FileFeature>()) ? 0 : 1;
    if (oldIndexOffset != m_indexOffset) {
        setChanged(Changes::StructureChanged);
    }
}

void babelwires::ContentsCache::updateModifierFlags() {
    if (!m_rows[0].getInputFeature()) {
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
    // The need for three variables here is a consequence of there being no root node in the Edit tree. Hmmm.
    auto editIt = m_edits.end();
    auto childBegin = m_edits.begin();
    auto childEnd = m_edits.end();

    for (int index = 0; index < m_rows.size(); ++index) {
        if (index > 0) {
            // See if there is an appropriate editTree iterator in the parent's set.
            stackDataStack.emplace_back(index);
            const PathStep& stepToHere = m_rows[index].m_path.getLastStep();
            StackData& parentStackData = stackDataStack[stackDataStack.size() - 2];
            const auto it = parentStackData.m_iteratorFromStep.find(stepToHere);
            if (it != parentStackData.m_iteratorFromStep.end()) {
                editIt = it->second;
                childBegin = editIt.childrenBegin();
                childEnd = editIt.childrenEnd();
                parentStackData.m_iteratorFromStep.erase(it);
            } else {
                editIt = m_edits.end();
                childBegin = m_edits.end();
                childEnd = m_edits.end();
            }
        }

        // Use the edit iterator for this row if there is one.
        {
            ContentsCacheEntry& currentRow = m_rows[index];
            if (editIt != m_edits.end()) {
                if (const Modifier* modifier = editIt.getModifier()) {
                    currentRow.m_hasModifier = true;
                    currentRow.m_hasLocalModifier = !modifier->asConnectionModifier();
                    currentRow.m_hasFailedModifier = modifier->isFailed();
                } else {
                    currentRow.m_hasModifier = false;
                }
            } else {
                currentRow.m_hasModifier = false;
                currentRow.m_hasLocalModifier = false;
                currentRow.m_hasFailedModifier = false;
            }
        }

        // Find the set of edits for children at this row.
        {
            StackData& stackData = stackDataStack.back();
            for (auto childIt = childBegin; childIt != childEnd; childIt.nextSibling()) {
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
    const int adjustedI = i + m_indexOffset;
    if ((0 <= adjustedI) && (adjustedI < m_rows.size())) {
        return &m_rows[adjustedI];
    }
    // TODO This is defensive. Is this necessary?
    return nullptr;
}

int babelwires::ContentsCache::getNumRows() const {
    return m_rows.size() - m_indexOffset;
}

int babelwires::ContentsCache::getIndexOfPath(bool seekInputFeature, const FeaturePath& path) const {
    for (int i = m_indexOffset; i < m_rows.size(); ++i) {
        const ContentsCacheEntry& entry = m_rows[i];
        if ((seekInputFeature && !entry.m_inputFeature) || (!seekInputFeature && !entry.m_outputFeature)) {
            continue;
        }
        if (path == m_rows[i].m_path) {
            return i - m_indexOffset;
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
