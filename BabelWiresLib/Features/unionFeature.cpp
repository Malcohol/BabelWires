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
    m_unselectedBranches.resize(m_tags.size());
}

void babelwires::UnionFeature::addFieldInBranchInternal(const Identifier& tag, FieldAndIndex fieldAndIndex) {
    assert(isTag(tag) && "The tag is not a valid tag for this union");
    const unsigned int index = getIndexOfTag(tag);
    // All branches are unselected until the union is set to default. This ensures the stored indices are correct.
    UnselectedBranch& unselectedBranch = m_unselectedBranches[index];
    unselectedBranch.m_inactiveFields.emplace_back(std::move(fieldAndIndex));
}

void babelwires::UnionFeature::selectTag(Identifier tag) {
    if (!isTag(tag)) {
        // TODO Look up name from identifier
        throw ModelException() << "\"" << tag << "\" is not a valid tag for this union";
    }

    const int indexOfNewTag = getIndexOfTag(tag);

    selectTagByIndex(indexOfNewTag);
}

void babelwires::UnionFeature::selectTagByIndex(unsigned int index) {
    if (index == m_selectedTagIndex) {
        // Nothing to do.
        return;
    }

    if (m_selectedTagIndex >= 0) {
        UnselectedBranch& newUnselectedBranch = m_unselectedBranches[m_selectedTagIndex];
        assert(newUnselectedBranch.m_inactiveFields.empty() && "The unselected branch object of the selected tag should be empty");

        // Deactivate the fields in the currently selected branch.

        // Maybe need to reverse?
        for (auto identifier : m_selectedBranch.m_activeFields) {
            FieldAndIndex fieldAndIndex = removeField(identifier);
            fieldAndIndex.m_feature->setToDefault();
            newUnselectedBranch.m_inactiveFields.emplace_back(std::move(fieldAndIndex));
        }

        m_selectedBranch.m_activeFields.clear();
    }

    // Activate the fields in the unselectedBranch.

    typeof(UnselectedBranch::m_inactiveFields) oldUnselectedFields;
    oldUnselectedFields.swap(m_unselectedBranches[index].m_inactiveFields);

    for (auto& fieldAndIndex : oldUnselectedFields) {
        m_selectedBranch.m_activeFields.emplace_back(fieldAndIndex.m_identifier);
        addFieldAndIndexInternal(std::move(fieldAndIndex));
    }

    m_selectedTagIndex = index;
}

const babelwires::UnionFeature::TagValues& babelwires::UnionFeature::getTags() const {
    return m_tags;
}

bool babelwires::UnionFeature::isTag(Identifier tag) const {
    return std::find(m_tags.begin(), m_tags.end(), tag) != m_tags.end();
}

void babelwires::UnionFeature::doSetToDefault() {
    setToDefaultNonRecursive();
    setSubfeaturesToDefault();
}

void babelwires::UnionFeature::doSetToDefaultNonRecursive() {
    for (auto& unselectedBranch : m_unselectedBranches) {
        for (auto& inactiveField : unselectedBranch.m_inactiveFields) {
            // After construction, these may not have been set to their default state.
            inactiveField.m_feature->setToDefault();
        }
    }
    selectTagByIndex(m_defaultTagIndex);
}

babelwires::Identifier babelwires::UnionFeature::getSelectedTag() const {
    assert((m_selectedTagIndex >= 0) && "Cannot call getSelectedTag until the union has been set to default");
    return m_tags[m_selectedTagIndex];
}

unsigned int babelwires::UnionFeature::getSelectedTagIndex() const {
    return m_selectedTagIndex;
}

unsigned int babelwires::UnionFeature::getIndexOfTag(Identifier tag) const {
    const auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    assert((it != m_tags.end()) && "The given tag is not a valid tag of this union");
    return it - m_tags.begin();
}
