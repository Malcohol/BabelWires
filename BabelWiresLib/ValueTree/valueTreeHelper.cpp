/**
 * A ValueTreeNode is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/ValueTree/valueTreeHelper.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>

std::tuple<const babelwires::ValueTreeNode*, unsigned int, babelwires::Range<unsigned int>, unsigned int>
babelwires::ValueTreeHelper::getInfoFromArrayFeature(const ValueTreeNode* f) {
    if (!f) {
        return {};
    }
    if (auto arrayType = f->getType().as<ArrayType>()) {
        return {f, arrayType->getNumChildren(f->getValue()), arrayType->getSizeRange(),
                arrayType->getInitialSize()};
    }
    return {};
}

std::tuple<const babelwires::ValueTreeNode*, std::map<babelwires::ShortId, bool>>
babelwires::ValueTreeHelper::getInfoFromRecordWithOptionalsFeature(const ValueTreeNode* f) {
    if (!f) {
        return {};
    }
    if (auto recordType = f->getType().as<RecordType>()) {
        std::map<ShortId, bool> currentlyActivatedOptionals;
        for (auto opt : recordType->getOptionalFieldIds()) {
            currentlyActivatedOptionals.insert(std::pair{ opt, recordType->isActivated(f->getValue(), opt)});
        }
        return { f, currentlyActivatedOptionals};
    }
    return {};
}

std::tuple<const babelwires::ValueTreeNode*, bool, std::vector<babelwires::ShortId>> babelwires::ValueTreeHelper::getInfoFromRecordWithVariantsFeature(const ValueTreeNode* f, std::optional<ShortId> tagId) {
    if (!f) { 
        return {};
    }
    if (auto recordWithVariantsType = f->getType().as<RecordWithVariantsType>()) {
        if (!tagId) {
            tagId = recordWithVariantsType->getDefaultTag();
        } else if (!recordWithVariantsType->isTag(*tagId)) {
            return {};
        }
        if (recordWithVariantsType->getSelectedTag(f->getValue()) == tagId) {
            return { f, true, {}};
        } else {
            return { f, false, recordWithVariantsType->getFieldsRemovedByChangeOfBranch(f->getValue(), *tagId) };
        }
    }
    return {};
}
