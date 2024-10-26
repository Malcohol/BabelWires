/**
 * Out-of-line utility functions used by instance methods.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Instance/instanceUtils.hpp>

#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

const babelwires::Feature& babelwires::InstanceUtils::getChild(const babelwires::Feature& recordFeature,
                                                            babelwires::ShortId id) {
    const int index = recordFeature.getChildIndexFromStep(babelwires::PathStep(id));
    assert(index >= 0);
    return recordFeature.getFeature(index)->is<babelwires::Feature>();
}

babelwires::Feature& babelwires::InstanceUtils::getChild(babelwires::Feature& recordFeature, babelwires::ShortId id) {
    const int index = recordFeature.getChildIndexFromStep(babelwires::PathStep(id));
    assert(index >= 0);
    return recordFeature.getFeature(index)->is<babelwires::Feature>();
}


const babelwires::Feature* babelwires::InstanceUtils::tryGetChild(const babelwires::Feature& recordFeature,
                                                               babelwires::ShortId id) {
    const int index = recordFeature.getChildIndexFromStep(babelwires::PathStep(id));
    if (index >= 0) {
        return &recordFeature.getFeature(index)->is<babelwires::Feature>();
    } else {
        return nullptr;
    }
}

babelwires::Feature& babelwires::InstanceUtils::activateAndGetChild(babelwires::Feature& recordFeature,
                                                                 babelwires::ShortId id) {
    const RecordType& recordType = recordFeature.getType().is<RecordType>();
    babelwires::ValueHolder recordValue = recordFeature.getValue();
    if (!recordType.isActivated(recordValue, id)) {
        const babelwires::TypeSystem& typeSystem = recordFeature.getTypeSystem();
        recordType.activateField(typeSystem, recordValue, id);
        recordFeature.setValue(recordValue);
    }
    return getChild(recordFeature, id);
}

void babelwires::InstanceUtils::deactivateChild(babelwires::Feature& recordFeature,
                                                                 babelwires::ShortId id) {
    const RecordType& recordType = recordFeature.getType().is<RecordType>();
    babelwires::ValueHolder recordValue = recordFeature.getValue();
    if (recordType.isActivated(recordValue, id)) {
        recordType.deactivateField(recordValue, id);
        recordFeature.setValue(recordValue);
    }
}

babelwires::ShortId babelwires::InstanceUtils::getSelectedTag(const Feature& valueFeature) {
    const RecordWithVariantsType& type = valueFeature.getType().is<RecordWithVariantsType>();
    const ValueHolder& value = valueFeature.getValue();
    return type.getSelectedTag(value);
}

void babelwires::InstanceUtils::selectTag(Feature& valueFeature, ShortId tag) {
    const RecordWithVariantsType& type = valueFeature.getType().is<RecordWithVariantsType>();
    ValueHolder value = valueFeature.getValue();
    if (tag != type.getSelectedTag(value)) {
        const babelwires::TypeSystem& typeSystem = valueFeature.getTypeSystem();
        type.selectTag(typeSystem, value, tag);
        valueFeature.setValue(value);
    }
}

unsigned int babelwires::InstanceUtils::getArraySize(const babelwires::Feature& arrayFeature) {
    return arrayFeature.getNumFeatures();
}

void babelwires::InstanceUtils::setArraySize(babelwires::Feature& arrayFeature, unsigned int newSize) {
    const auto& type = arrayFeature.getType().is<babelwires::ArrayType>();
    const auto& typeSystem = arrayFeature.getTypeSystem();
    babelwires::ValueHolder value = arrayFeature.getValue();
    value.copyContentsAndGetNonConst();
    type.setSize(typeSystem, value, newSize);
    arrayFeature.setValue(value);
}

const babelwires::Feature& babelwires::InstanceUtils::getChild(const babelwires::Feature& arrayFeature, unsigned int index) {
    return arrayFeature.getFeature(index)->is<babelwires::Feature>();
}

babelwires::Feature& babelwires::InstanceUtils::getChild(babelwires::Feature& arrayFeature, unsigned int index) {
    return arrayFeature.getFeature(index)->is<babelwires::Feature>();
}
