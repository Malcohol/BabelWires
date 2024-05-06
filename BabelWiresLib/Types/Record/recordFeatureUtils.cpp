/**
 * A set of useful functions for interacting with features of recordType.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Types/Record/recordFeatureUtils.hpp>

#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>

const babelwires::ValueFeature& babelwires::RecordFeatureUtils::getChild(const babelwires::ValueFeature& recordFeature,
                                                            babelwires::ShortId id) {
    const int index = recordFeature.getChildIndexFromStep(babelwires::PathStep(id));
    assert(index >= 0);
    return recordFeature.getFeature(index)->is<babelwires::ValueFeature>();
}

babelwires::ValueFeature& babelwires::RecordFeatureUtils::getChild(babelwires::ValueFeature& recordFeature, babelwires::ShortId id) {
    const int index = recordFeature.getChildIndexFromStep(babelwires::PathStep(id));
    assert(index >= 0);
    return recordFeature.getFeature(index)->is<babelwires::ValueFeature>();
}

const babelwires::ValueFeature* babelwires::RecordFeatureUtils::tryGetChild(const babelwires::ValueFeature& recordFeature,
                                                               babelwires::ShortId id) {
    const int index = recordFeature.getChildIndexFromStep(babelwires::PathStep(id));
    if (index >= 0) {
        return &recordFeature.getFeature(index)->is<babelwires::ValueFeature>();
    } else {
        return nullptr;
    }
}

babelwires::ValueFeature& babelwires::RecordFeatureUtils::activateAndGetChild(babelwires::ValueFeature& recordFeature,
                                                                 babelwires::ShortId id) {
    const RecordType& recordType = recordFeature.getType().is<RecordType>();
    babelwires::ValueHolder recordValue = recordFeature.getValue();
    if (!recordType.isActivated(recordValue, id)) {
        const babelwires::TypeSystem& typeSystem = babelwires::RootFeature::getTypeSystemAt(recordFeature);
        recordType.activateField(typeSystem, recordValue, id);
        recordFeature.setValue(recordValue);
    }
    return getChild(recordFeature, id);
}
