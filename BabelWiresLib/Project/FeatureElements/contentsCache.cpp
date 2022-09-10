/**
 * The ContentsCache summarizes the contents of a FeatureElement visible to the user.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/FeatureElements/contentsCache.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/recordFeature.hpp>
#include <BabelWiresLib/FileFormat/fileFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/editTree.hpp>
#include <BabelWiresLib/Project/Modifiers/modifier.hpp>
#include <BabelWiresLib/ValueNames/valueNames.hpp>

#include <Common/Identifiers/identifierRegistry.hpp>

#include <unordered_set>

babelwires::ContentsCacheEntry::ContentsCacheEntry(std::string label, const Feature* inputFeature,
                                                   const Feature* outputFeature, const FeaturePath& path)
    : m_label(std::move(label))
    , m_inputFeature(inputFeature)
    , m_outputFeature(outputFeature)
    , m_path(path)
    , m_isExpandable(false)
    , m_isExpanded(false)
    , m_hasModifier(false)
    , m_hasLocalModifier(false)
    , m_hasFailedModifier(false)
    , m_hasHiddenModifiers(false)
    , m_hasFailedHiddenModifiers(false)
    , m_hasSubModifiers(false) {}

babelwires::ContentsCache::ContentsCache(const EditTree& edits)
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
            ContentsCacheBuilder(std::vector<ContentsCacheEntry>& rows, const EditTree& edits)
                : m_identifierRegistry(IdentifierRegistry::read())
                , m_rows(rows)
                , m_edits(edits) {}

            /// Sets some compound details and returns whether the compound is expanded or not.
            bool setAndGetCompoundIsExpanded(std::uint8_t index, const FeaturePath& path, bool isExpandable) {
                if (index == 0) {
                    // Special case: The top level cannot be collapsed (mainly because the edit tree has no top level).
                    m_rows[index].m_isExpandable = false;
                } else {
                    m_rows[index].m_isExpandable = isExpandable;
                    if (isExpandable) {
                        m_rows[index].m_isExpanded = m_edits.isExpanded(path);
                        return m_rows[index].m_isExpanded;
                    }
                }
                return true;
            }

            void addInputFeatureToCache(std::string label, const babelwires::Feature* f, const FeaturePath& path) {
                m_rows.emplace_back(ContentsCacheEntry(std::move(label), f, nullptr, path));
                const std::uint8_t thisIndex = m_rows.size() - 1;
                if (const auto* record = f->as<const babelwires::RecordFeature>()) {
                    addInputRecordFeatureToCache(record, path, thisIndex);
                } else if (const auto* array = f->as<const babelwires::ArrayFeature>()) {
                    addInputArrayFeatureToCache(array, path, thisIndex);
                }
            }

            void addInputRecordFeatureToCache(const babelwires::RecordFeature* record, const FeaturePath& path,
                                              std::uint8_t thisIndex) {
                if (setAndGetCompoundIsExpanded(thisIndex, path, record->getNumFeatures())) {
                    for (int i = 0; i < record->getNumFeatures(); ++i) {
                        FeaturePath pathToChild = path;
                        pathToChild.pushStep(PathStep(record->getFieldIdentifier(i)));
                        addInputFeatureToCache(m_identifierRegistry->getName(record->getFieldIdentifier(i)),
                                               record->getFeature(i), std::move(pathToChild));
                    }
                }
            }

            void addInputArrayFeatureToCache(const babelwires::ArrayFeature* array, const FeaturePath& path,
                                             std::uint8_t thisIndex) {
                if (setAndGetCompoundIsExpanded(thisIndex, path, array->getNumFeatures())) {
                    const babelwires::ValueNames* const entryNames = array->getEntryNames();
                    for (int i = 0; i < array->getNumFeatures(); ++i) {
                        FeaturePath pathToChild = path;
                        pathToChild.pushStep(PathStep(i));
                        addInputFeatureToCache(getArrayEntryLabel(i, entryNames), array->getFeature(i),
                                               std::move(pathToChild));
                    }
                }
            }

            void addOutputFeatureToCache(std::string label, const babelwires::Feature* f, const FeaturePath& path) {
                m_rows.emplace_back(ContentsCacheEntry(std::move(label), nullptr, f, path));
                const std::uint8_t thisIndex = m_rows.size() - 1;
                if (const auto* record = f->as<const babelwires::RecordFeature>()) {
                    addOutputRecordFeatureToCache(record, path, thisIndex);
                } else if (const auto* array = f->as<const babelwires::ArrayFeature>()) {
                    addOutputArrayFeatureToCache(array, path, thisIndex);
                }
            }

            void addOutputRecordFeatureToCache(const babelwires::RecordFeature* record, const FeaturePath& path,
                                               std::uint8_t thisIndex) {
                if (setAndGetCompoundIsExpanded(thisIndex, path, record->getNumFeatures())) {
                    for (int i = 0; i < record->getNumFeatures(); ++i) {
                        FeaturePath pathToChild = path;
                        pathToChild.pushStep(PathStep(record->getFieldIdentifier(i)));
                        addOutputFeatureToCache(m_identifierRegistry->getName(record->getFieldIdentifier(i)),
                                                record->getFeature(i), std::move(pathToChild));
                    }
                }
            }

            void addOutputArrayFeatureToCache(const babelwires::ArrayFeature* array, const FeaturePath& path,
                                              std::uint8_t thisIndex) {
                if (setAndGetCompoundIsExpanded(thisIndex, path, array->getNumFeatures())) {
                    const babelwires::ValueNames* const entryNames = array->getEntryNames();
                    for (int i = 0; i < array->getNumFeatures(); ++i) {
                        FeaturePath pathToChild = path;
                        pathToChild.pushStep(PathStep(i));
                        addOutputFeatureToCache(getArrayEntryLabel(i, entryNames), array->getFeature(i),
                                                std::move(pathToChild));
                    }
                }
            }

            void addFeatureToCache(std::string label, const Feature* inputFeature, const Feature* outputFeature,
                                   const FeaturePath& path) {
                m_rows.emplace_back(
                    ContentsCacheEntry(std::move(label), inputFeature, outputFeature, path));
                const std::uint8_t thisIndex = m_rows.size() - 1;
                if (const auto* const inputRecord = inputFeature->as<const babelwires::RecordFeature>()) {
                    const auto* const outputRecord = outputFeature->as<const babelwires::RecordFeature>();
                    assert(outputRecord && "If the input is a record, the output must be a record too");
                    addRecordContentsToCache(inputRecord, outputRecord, path, thisIndex);
                } else if (const auto* const inputArray = inputFeature->as<const babelwires::ArrayFeature>()) {
                    const auto* const outputArray = outputFeature->as<const babelwires::ArrayFeature>();
                    assert(outputArray && "If the input is an array, the output must be an array too");
                    addArrayContentsToCache(inputArray, outputArray, path, thisIndex);
                }
            }

            void addRecordContentsToCache(const RecordFeature* inputFeature, const RecordFeature* outputFeature,
                                          const FeaturePath& path, std::uint8_t thisIndex) {
                if (setAndGetCompoundIsExpanded(thisIndex, path,
                                                inputFeature->getNumFeatures() + outputFeature->getNumFeatures())) {
                    const int cacheStartSize = m_rows.size();
                    std::unordered_set<int> outputIndicesHandled;
                    for (int i = 0; i < inputFeature->getNumFeatures(); ++i) {
                        Identifier stepToChild = inputFeature->getFieldIdentifier(i);
                        FeaturePath pathToChild = path;
                        const int outputChildIndex = outputFeature->getChildIndexFromStep(stepToChild);
                        pathToChild.pushStep(PathStep(stepToChild));
                        if (outputChildIndex >= 0) {
                            addFeatureToCache(m_identifierRegistry->getName(inputFeature->getFieldIdentifier(i)),
                                              inputFeature->getFeature(i), outputFeature->getFeature(outputChildIndex),
                                              std::move(pathToChild));
                            outputIndicesHandled.insert(outputChildIndex);
                        } else {
                            addInputFeatureToCache(m_identifierRegistry->getName(inputFeature->getFieldIdentifier(i)),
                                                   inputFeature->getFeature(i), std::move(pathToChild));
                        }
                    }
                    for (int i = 0; i < outputFeature->getNumFeatures(); ++i) {
                        if (outputIndicesHandled.find(i) == outputIndicesHandled.end()) {
                            FeaturePath pathToChild = path;
                            pathToChild.pushStep(PathStep(outputFeature->getFieldIdentifier(i)));
                            addOutputFeatureToCache(m_identifierRegistry->getName(outputFeature->getFieldIdentifier(i)),
                                                    outputFeature->getFeature(i), std::move(pathToChild));
                        }
                    }
                }
            }

            void addArrayContentsToCache(const ArrayFeature* inputFeature, const ArrayFeature* outputFeature,
                                         const FeaturePath& path, std::uint8_t thisIndex) {
                if (setAndGetCompoundIsExpanded(thisIndex, path,
                                                inputFeature->getNumFeatures() + outputFeature->getNumFeatures())) {
                    int i = 0;
                    for (; i < std::min(inputFeature->getNumFeatures(), outputFeature->getNumFeatures()); ++i) {
                        const babelwires::ValueNames* entryNames = inputFeature->getEntryNames();
                        entryNames = entryNames ? entryNames : outputFeature->getEntryNames();
                        FeaturePath pathToChild = path;
                        pathToChild.pushStep(PathStep(i));
                        addFeatureToCache(getArrayEntryLabel(i, entryNames), inputFeature->getFeature(i),
                                          outputFeature->getFeature(i), std::move(pathToChild));
                    }
                    for (; i < inputFeature->getNumFeatures(); ++i) {
                        const babelwires::ValueNames* const entryNames = inputFeature->getEntryNames();
                        FeaturePath pathToChild = path;
                        pathToChild.pushStep(PathStep(i));
                        addInputFeatureToCache(getArrayEntryLabel(i, entryNames), inputFeature->getFeature(i),
                                               std::move(pathToChild));
                    }
                    for (; i < outputFeature->getNumFeatures(); ++i) {
                        const babelwires::ValueNames* const entryNames = outputFeature->getEntryNames();
                        FeaturePath pathToChild = path;
                        pathToChild.pushStep(PathStep(i));
                        addOutputFeatureToCache(getArrayEntryLabel(i, entryNames), outputFeature->getFeature(i),
                                                std::move(pathToChild));
                    }
                }
            }

            IdentifierRegistry::ReadAccess m_identifierRegistry;
            std::vector<ContentsCacheEntry>& m_rows;
            const EditTree& m_edits;
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
        builder.addFeatureToCache(rootLabel, inputFeature, outputFeature, FeaturePath());
    } else if (inputFeature) {
        builder.addInputFeatureToCache(rootLabel, inputFeature, FeaturePath());
    } else if (outputFeature) {
        builder.addOutputFeatureToCache(rootLabel, outputFeature, FeaturePath());
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

    using Iterator = EditTree::Iterator<const EditTree>;

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
            // See if there is an approrpriate editTree iterator in the parent's set.
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
