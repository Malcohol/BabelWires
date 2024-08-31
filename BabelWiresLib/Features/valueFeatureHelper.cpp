/**
 * A ValueFeature is a feature which provides access to a value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Features/valueFeatureHelper.hpp>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Features/recordWithOptionalsFeature.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsValue.hpp>

std::tuple<const babelwires::CompoundFeature*, unsigned int, babelwires::Range<unsigned int>, unsigned int>
babelwires::ValueFeatureHelper::getInfoFromArrayFeature(const Feature* f) {
    if (!f) {
        return {};
    }
    if (auto arrayFeature = f->as<const ArrayFeature>()) {
        return {arrayFeature, arrayFeature->getNumFeatures(), arrayFeature->getSizeRange(),
                arrayFeature->getSizeRange().m_min};
    } else if (auto valueFeature = f->as<const ValueFeature>()) {
        if (auto arrayType = valueFeature->getType().as<ArrayType>()) {
            return {valueFeature, arrayType->getNumChildren(valueFeature->getValue()), arrayType->getSizeRange(),
                    arrayType->getInitialSize()};
        }
    }
    return {};
}

std::tuple<const babelwires::CompoundFeature*, bool>
babelwires::ValueFeatureHelper::getInfoFromRecordWithOptionalsFeature(const Feature* f, ShortId optionalId) {
    if (!f) {
        return {};
    }
    if (auto recordWithOptionalsFeature = f->as<const RecordWithOptionalsFeature>()) {
        if (!recordWithOptionalsFeature->isOptional(optionalId)) {
            return {};
        }
        return { recordWithOptionalsFeature, recordWithOptionalsFeature->isActivated(optionalId) };
    } else if (auto valueFeature = f->as<const ValueFeature>()) {
        if (auto recordType = valueFeature->getType().as<RecordType>()) {
            if (!recordType->isOptional(optionalId)) {
                return {};
            } else {
                return { valueFeature, recordType->isActivated(valueFeature->getValue(), optionalId)};
            }
        }
    }
    return {};
}

std::tuple<const babelwires::CompoundFeature*, bool, std::vector<babelwires::ShortId>> babelwires::ValueFeatureHelper::getInfoFromRecordWithVariantsFeature(const Feature* f, ShortId tagId) {
    if (!f) { 
        return {};
    }
    if (auto valueFeature = f->as<const ValueFeature>()) {
        if (auto recordWithVariantsType = valueFeature->getType().as<RecordWithVariantsType>()) {
            if (!recordWithVariantsType->isTag(tagId)) {
                return {};
            }
            if (recordWithVariantsType->getSelectedTag(valueFeature->getValue()) == tagId) {
                return { valueFeature, true, {}};
            } else {
                return { valueFeature, false, recordWithVariantsType->getFieldsRemovedByChangeOfBranch(valueFeature->getValue(), tagId) };
            }
        }
    }
    return {};
}
