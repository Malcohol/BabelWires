/**
 * Out-of-line utility functions used by instance methods.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Instance/instanceUtils.hpp>

#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

const babelwires::ValueFeature& babelwires::InstanceUtils::getChild(const babelwires::ValueFeature& recordFeature,
                                                            babelwires::ShortId id) {
    const int index = recordFeature.getChildIndexFromStep(babelwires::PathStep(id));
    assert(index >= 0);
    return recordFeature.getFeature(index)->is<babelwires::ValueFeature>();
}

babelwires::ValueFeature& babelwires::InstanceUtils::getChild(babelwires::ValueFeature& recordFeature, babelwires::ShortId id) {
    const int index = recordFeature.getChildIndexFromStep(babelwires::PathStep(id));
    assert(index >= 0);
    return recordFeature.getFeature(index)->is<babelwires::ValueFeature>();
}


const babelwires::ValueFeature* babelwires::InstanceUtils::tryGetChild(const babelwires::ValueFeature& recordFeature,
                                                               babelwires::ShortId id) {
    const int index = recordFeature.getChildIndexFromStep(babelwires::PathStep(id));
    if (index >= 0) {
        return &recordFeature.getFeature(index)->is<babelwires::ValueFeature>();
    } else {
        return nullptr;
    }
}

babelwires::ValueFeature& babelwires::InstanceUtils::activateAndGetChild(babelwires::ValueFeature& recordFeature,
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

void babelwires::InstanceUtils::deactivateChild(babelwires::ValueFeature& recordFeature,
                                                                 babelwires::ShortId id) {
    const RecordType& recordType = recordFeature.getType().is<RecordType>();
    babelwires::ValueHolder recordValue = recordFeature.getValue();
    if (recordType.isActivated(recordValue, id)) {
        recordType.deactivateField(recordValue, id);
        recordFeature.setValue(recordValue);
    }
}

babelwires::ShortId babelwires::InstanceUtils::getSelectedTag(const ValueFeature& valueFeature) {
    const RecordWithVariantsType& type = valueFeature.getType().is<RecordWithVariantsType>();
    const ValueHolder& value = valueFeature.getValue();
    return type.getSelectedTag(value);
}

void babelwires::InstanceUtils::selectTag(ValueFeature& valueFeature, ShortId tag) {
    const RecordWithVariantsType& type = valueFeature.getType().is<RecordWithVariantsType>();
    ValueHolder value = valueFeature.getValue();
    if (tag != type.getSelectedTag(value)) {
        const babelwires::TypeSystem& typeSystem = valueFeature.getTypeSystem();
        type.selectTag(typeSystem, value, tag);
        valueFeature.setValue(value);
    }
}

unsigned int babelwires::InstanceUtils::getArraySize(const babelwires::ValueFeature& arrayFeature) {
    return arrayFeature.getNumFeatures();
}

void babelwires::InstanceUtils::setArraySize(babelwires::ValueFeature& arrayFeature, unsigned int newSize) {
    const auto& type = arrayFeature.getType().is<babelwires::ArrayType>();
    const auto& typeSystem = arrayFeature.getTypeSystem();
    babelwires::ValueHolder value = arrayFeature.getValue();
    value.copyContentsAndGetNonConst();
    type.setSize(typeSystem, value, newSize);
    arrayFeature.setValue(value);
}

const babelwires::ValueFeature& babelwires::InstanceUtils::getChild(const babelwires::ValueFeature& arrayFeature, unsigned int index) {
    return arrayFeature.getFeature(index)->is<babelwires::ValueFeature>();
}

babelwires::ValueFeature& babelwires::InstanceUtils::getChild(babelwires::ValueFeature& arrayFeature, unsigned int index) {
    return arrayFeature.getFeature(index)->is<babelwires::ValueFeature>();
}
