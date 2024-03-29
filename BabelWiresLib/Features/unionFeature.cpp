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
    , m_defaultTagIndex(defaultTagIndex) {
    assert((m_defaultTagIndex < m_tags.size()) && "defaultTagIndex is out of range");
    assert(std::all_of(m_tags.begin(), m_tags.end(), [](const auto& t) { return t.getDiscriminator() != 0; }) &&
           "Union tags must be registered identifiers");
    m_fieldsInBranches.resize(m_tags.size());
}

void babelwires::UnionFeature::addFieldInBranchesInternal(const std::vector<ShortId>& tags, Field field) {
    assert((tags.size() > 0) && "Each branch field must be associated with at least one tag");
    assert((tryGetChildFromStep(PathStep{field.m_identifier}) == nullptr) &&
           "Field identifier already used in the main record");
    assert((m_fieldInfo.find(field.m_identifier) == m_fieldInfo.end()) &&
           "Field identifier already used in another branch");

    const int numRecordFeatures = getNumFeatures();
    auto& fieldInfo = m_fieldInfo[field.m_identifier];
    fieldInfo.m_feature = std::move(field.m_feature);
    // This ensures that features which need to access the RootFeature can do so,
    // even if they are in an unselected branch.
    fieldInfo.m_feature->setOwner(this);
    for (const auto& tag : tags) {
        unsigned int tagIndex = getIndexOfTag(tag);
        auto& tagToBranchFieldIndices = m_fieldsInBranches[tagIndex];
        fieldInfo.m_tagsWithIntendedIndices.emplace_back(TagIndexAndIntendedFieldIndex{
            tagIndex, numRecordFeatures + static_cast<int>(tagToBranchFieldIndices.size())});
        tagToBranchFieldIndices.emplace_back(field.m_identifier);
    }
}

void babelwires::UnionFeature::selectTag(ShortId tag) {
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

    BranchAdjustment adjustment = getBranchAdjustment(index);

    for (auto fieldIdentifier : adjustment.m_fieldsToRemove) {
        FieldInfo& fieldInfo = m_fieldInfo[fieldIdentifier];
        assert((fieldInfo.m_feature == nullptr) && "The fieldInfo of an active field must be unset");
        FieldAndIndex fieldAndIndex = removeField(fieldIdentifier);
        fieldInfo.m_feature.swap(fieldAndIndex.m_feature);
        fieldInfo.m_feature->setToDefault();
    }

    for (auto fieldIdentifier : adjustment.m_fieldsToAdd) {
        FieldInfo& fieldInfo = m_fieldInfo[fieldIdentifier];
        assert((fieldInfo.m_feature != nullptr) && "The fieldInfo of an inactive field must be set");

        auto it = std::find_if(fieldInfo.m_tagsWithIntendedIndices.begin(), fieldInfo.m_tagsWithIntendedIndices.end(),
                            [index](const auto& tagAndField) { return tagAndField.m_tagIndex == index; });
        assert((it != fieldInfo.m_tagsWithIntendedIndices.end()) && "The tag does not have this field in its branch");
        addFieldAndIndexInternal(FieldAndIndex{fieldIdentifier, std::move(fieldInfo.m_feature), it->m_fieldIndex});
    }

    m_selectedTagIndex = index;
}

const babelwires::UnionFeature::TagValues& babelwires::UnionFeature::getTags() const {
    return m_tags;
}

bool babelwires::UnionFeature::isTag(ShortId tag) const {
    return std::find(m_tags.begin(), m_tags.end(), tag) != m_tags.end();
}

void babelwires::UnionFeature::doSetToDefault() {
    if (m_selectedTagIndex == -1) {
        // This ensures that unselected tags are definitely defaulted.
        // We do this here, to ensure the class is fully set up first.
        for (auto& it : m_fieldInfo) {
            it.second.m_feature->setToDefault();
        }
    }
    setSubfeaturesToDefault();
    // Doing this second avoids setting the default branch to its default value twice.
    setToDefaultNonRecursive();
}

void babelwires::UnionFeature::doSetToDefaultNonRecursive() {
    selectTagByIndex(m_defaultTagIndex);
}

babelwires::ShortId babelwires::UnionFeature::getSelectedTag() const {
    assert((m_selectedTagIndex >= 0) && "Cannot call getSelectedTag until the union has been set to default");
    return m_tags[m_selectedTagIndex];
}

unsigned int babelwires::UnionFeature::getSelectedTagIndex() const {
    return m_selectedTagIndex;
}

unsigned int babelwires::UnionFeature::getIndexOfTag(ShortId tag) const {
    const auto it = std::find(m_tags.begin(), m_tags.end(), tag);
    assert((it != m_tags.end()) && "The given tag is not a valid tag of this union");
    return it - m_tags.begin();
}

std::vector<babelwires::ShortId>
babelwires::UnionFeature::getFieldsRemovedByChangeOfBranch(ShortId proposedTag) const {
    unsigned int tagIndex = getIndexOfTag(proposedTag);
    babelwires::UnionFeature::BranchAdjustment branchAdjustment = getBranchAdjustment(tagIndex);
    return std::move(branchAdjustment.m_fieldsToRemove);
}

babelwires::UnionFeature::BranchAdjustment babelwires::UnionFeature::getBranchAdjustment(unsigned int tagIndex) const {
    assert((tagIndex != m_selectedTagIndex) && "Same branch");
    std::vector<ShortId> fieldsToRemove;
    if (m_selectedTagIndex >= 0) {
        fieldsToRemove = m_fieldsInBranches[m_selectedTagIndex];
    }
    const std::vector<ShortId>& targetBranch = m_fieldsInBranches[tagIndex];
    std::vector<ShortId> fieldsToAdd = targetBranch;
    // This doesn't need to be N^2, but these vectors are likely to be very small, so it's fine.
    fieldsToAdd.erase(std::remove_if(fieldsToAdd.begin(), fieldsToAdd.end(),
                                   [&fieldsToRemove](auto t) {
                                       return std::find(fieldsToRemove.begin(), fieldsToRemove.end(), t) !=
                                              fieldsToRemove.end();
                                   }),
                    fieldsToAdd.end());
    fieldsToRemove.erase(std::remove_if(fieldsToRemove.begin(), fieldsToRemove.end(),
                                       [&targetBranch](auto t) {
                                           return std::find(targetBranch.begin(), targetBranch.end(), t) != targetBranch.end();
                                       }),
                        fieldsToRemove.end());
    return BranchAdjustment{fieldsToRemove, fieldsToAdd};
}
