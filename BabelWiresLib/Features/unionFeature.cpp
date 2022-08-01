/**
 * A UnionFeature is a Record feature with alternative sets of features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/unionFeature.hpp>

#include <BabelWiresLib/Features/modelExceptions.hpp>

babelwires::UnionFeature::UnionFeature(TagValues tags, unsigned int defaultTagIndex) 
    : m_tags(std::move(tags))
    , m_defaultTagIndex(defaultTagIndex)
{
    assert((m_defaultTagIndex < m_tags.size()) && "defaultTagIndex is out of range");
}

void babelwires::UnionFeature::addFieldInBranchInternal(const Identifier& tag, FieldAndIndex fieldAndIndex) {
    assert(isTag(tag) && "The tag is not a valid tag for this union");
    // All branches are unselected until the union is set to default. This ensures the stored indices are correct.
    UnselectedBranch& unselectedBranch = m_unselectedBranches[tag];
    unselectedBranch.m_inactiveFields.emplace_back(std::move(fieldAndIndex));
}

void babelwires::UnionFeature::selectTag(Identifier tag) {
    if (!isTag(tag)) {
        // TODO Look up name from identifier
        throw ModelException() << "\"" << tag << "\" is not a valid tag for this union";
    }
    if (isSelectedTag(tag)) {
        // Nothing to do.
        return;
    }

    if (m_selectedTag.has_value()) {
        // There is at an active branch (always true after setToDefault has been called).
        assert((m_unselectedBranches.size() == m_tags.size() - 1) && "Inconsistency between m_unselectedBranches and m_tags");

        // Deactivate the fields in the currently selected branch.
        UnselectedBranch newUnselectedBranch;
       
        // Maybe need to reverse?
        for (auto identifier : m_selectedBranch.m_activeFields) {
            FieldAndIndex fieldAndIndex = removeField(identifier);
            fieldAndIndex.m_feature->setToDefault();
            newUnselectedBranch.m_inactiveFields.emplace_back(std::move(fieldAndIndex));
        }

        m_selectedBranch.m_activeFields.clear();
        m_unselectedBranches.insert(std::pair(tag, std::move(newUnselectedBranch)));
    } else {
        assert((m_unselectedBranches.size() == m_tags.size()) && "Inconsistency between m_unselectedBranches and m_tags");
    }

    // Activate the fields in the unselectedBranch.

    auto it = m_unselectedBranches.find(tag);
    assert((it != m_unselectedBranches.end()) && "Tag not found in m_unselectedBranches");
    UnselectedBranch oldUnselectedBranch = std::move(it->second);
    m_unselectedBranches.erase(it);

    for (auto& fieldAndIndex : oldUnselectedBranch.m_inactiveFields) {
        m_selectedBranch.m_activeFields.emplace_back(fieldAndIndex.m_identifier);
        addFieldAndIndexInternal(std::move(fieldAndIndex));
    }

    m_selectedTag = tag;
}

const babelwires::UnionFeature::TagValues& babelwires::UnionFeature::getTags() const {
    return m_tags;
}

bool babelwires::UnionFeature::isTag(Identifier tag) const {
    return std::find(m_tags.begin(), m_tags.end(), tag) != m_tags.end();
}

bool babelwires::UnionFeature::isSelectedTag(Identifier tag) const {
    assert(isTag(tag) && "isSelectedTag can only be called with an actual tag");
    const bool isGivenTag = (m_selectedTag == tag);
    assert(isGivenTag == (m_unselectedBranches.find(tag) != m_unselectedBranches.end()) && "Inconsistency between m_selectedTag and m_unselectedBranches");
    return isGivenTag;
}

void babelwires::UnionFeature::doSetToDefault() {
    setToDefaultNonRecursive();
    setSubfeaturesToDefault();
}

void babelwires::UnionFeature::doSetToDefaultNonRecursive() {
    for (auto& pair : m_unselectedBranches) {
        for (auto& inactiveField : pair.second.m_inactiveFields) {
            // After construction, these may not have been set to their default state.
            inactiveField.m_feature->setToDefault();
        }
    }
    selectTag(m_tags[m_defaultTagIndex]);
}

babelwires::Identifier babelwires::UnionFeature::getSelectedTag() const {
    assert(m_selectedTag.has_value() && "Cannot call getSelectedTag until the union has been set to default");
    return *m_selectedTag;
}
