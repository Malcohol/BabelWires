/**
 * A Feature is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/valueFeatureHelper.hpp>

#include <BabelWiresLib/Features/feature.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>

std::tuple<const babelwires::Feature*, unsigned int, babelwires::Range<unsigned int>, unsigned int>
babelwires::ValueFeatureHelper::getInfoFromArrayFeature(const Feature* f) {
    if (!f) {
        return {};
    }
    if (auto valueFeature = f->as<const Feature>()) {
        if (auto arrayType = valueFeature->getType().as<ArrayType>()) {
            return {valueFeature, arrayType->getNumChildren(valueFeature->getValue()), arrayType->getSizeRange(),
                    arrayType->getInitialSize()};
        }
    }
    return {};
}

std::tuple<const babelwires::Feature*, std::map<babelwires::ShortId, bool>>
babelwires::ValueFeatureHelper::getInfoFromRecordWithOptionalsFeature(const Feature* f) {
    if (!f) {
        return {};
    }
    if (auto valueFeature = f->as<const Feature>()) {
        if (auto recordType = valueFeature->getType().as<RecordType>()) {
            std::map<ShortId, bool> currentlyActivatedOptionals;
            for (auto opt : recordType->getOptionalFieldIds()) {
                currentlyActivatedOptionals.insert(std::pair{ opt, recordType->isActivated(valueFeature->getValue(), opt)});
            }
            return { valueFeature, currentlyActivatedOptionals};
        }
    }
    return {};
}

std::tuple<const babelwires::Feature*, bool, std::vector<babelwires::ShortId>> babelwires::ValueFeatureHelper::getInfoFromRecordWithVariantsFeature(const Feature* f, std::optional<ShortId> tagId) {
    if (!f) { 
        return {};
    }
    if (auto valueFeature = f->as<const Feature>()) {
        if (auto recordWithVariantsType = valueFeature->getType().as<RecordWithVariantsType>()) {
            if (!tagId) {
                tagId = recordWithVariantsType->getDefaultTag();
            } else if (!recordWithVariantsType->isTag(*tagId)) {
                return {};
            }
            if (recordWithVariantsType->getSelectedTag(valueFeature->getValue()) == tagId) {
                return { valueFeature, true, {}};
            } else {
                return { valueFeature, false, recordWithVariantsType->getFieldsRemovedByChangeOfBranch(valueFeature->getValue(), *tagId) };
            }
        }
    }
    return {};
}
