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

const babelwires::ValueTreeNode& babelwires::InstanceUtils::getChild(const babelwires::ValueTreeNode& recordTreeNode,
                                                            babelwires::ShortId id) {
    const int index = recordTreeNode.getChildIndexFromStep(id);
    assert(index >= 0);
    return recordTreeNode.getChild(index)->as<babelwires::ValueTreeNode>();
}

babelwires::ValueTreeNode& babelwires::InstanceUtils::getChild(babelwires::ValueTreeNode& recordTreeNode, babelwires::ShortId id) {
    const int index = recordTreeNode.getChildIndexFromStep(id);
    assert(index >= 0);
    return recordTreeNode.getChild(index)->as<babelwires::ValueTreeNode>();
}


const babelwires::ValueTreeNode* babelwires::InstanceUtils::tryGetChild(const babelwires::ValueTreeNode& recordTreeNode,
                                                               babelwires::ShortId id) {
    const int index = recordTreeNode.getChildIndexFromStep(id);
    if (index >= 0) {
        return &recordTreeNode.getChild(index)->as<babelwires::ValueTreeNode>();
    } else {
        return nullptr;
    }
}

babelwires::ValueTreeNode& babelwires::InstanceUtils::activateAndGetChild(babelwires::ValueTreeNode& recordTreeNode,
                                                                 babelwires::ShortId id) {
    const RecordType& recordType = recordTreeNode.getType()->as<RecordType>();
    babelwires::ValueHolder recordValue = recordTreeNode.getValue();
    if (!recordType.isActivated(recordValue, id)) {
        const babelwires::TypeSystem& typeSystem = recordTreeNode.getTypeSystem();
        recordType.activateField(typeSystem, recordValue, id);
        recordTreeNode.setValue(recordValue);
    }
    return getChild(recordTreeNode, id);
}

void babelwires::InstanceUtils::deactivateChild(babelwires::ValueTreeNode& recordTreeNode,
                                                                 babelwires::ShortId id) {
    const RecordType& recordType = recordTreeNode.getType()->as<RecordType>();
    babelwires::ValueHolder recordValue = recordTreeNode.getValue();
    if (recordType.isActivated(recordValue, id)) {
        recordType.deactivateField(recordValue, id);
        recordTreeNode.setValue(recordValue);
    }
}

babelwires::ShortId babelwires::InstanceUtils::getSelectedTag(const ValueTreeNode& valueTreeNode) {
    const RecordWithVariantsType& type = valueTreeNode.getType()->as<RecordWithVariantsType>();
    const ValueHolder& value = valueTreeNode.getValue();
    return type.getSelectedTag(value);
}

void babelwires::InstanceUtils::selectTag(ValueTreeNode& valueTreeNode, ShortId tag) {
    const RecordWithVariantsType& type = valueTreeNode.getType()->as<RecordWithVariantsType>();
    ValueHolder value = valueTreeNode.getValue();
    if (tag != type.getSelectedTag(value)) {
        const babelwires::TypeSystem& typeSystem = valueTreeNode.getTypeSystem();
        type.selectTag(typeSystem, value, tag);
        valueTreeNode.setValue(value);
    }
}

unsigned int babelwires::InstanceUtils::getArraySize(const babelwires::ValueTreeNode& arrayTreeNode) {
    return arrayTreeNode.getNumChildren();
}

void babelwires::InstanceUtils::setArraySize(babelwires::ValueTreeNode& arrayTreeNode, unsigned int newSize) {
    const auto& type = arrayTreeNode.getType()->as<babelwires::ArrayType>();
    const auto& typeSystem = arrayTreeNode.getTypeSystem();
    babelwires::ValueHolder value = arrayTreeNode.getValue();
    value.copyContentsAndGetNonConst();
    type.setSize(typeSystem, value, newSize);
    arrayTreeNode.setValue(value);
}

const babelwires::ValueTreeNode& babelwires::InstanceUtils::getChild(const babelwires::ValueTreeNode& arrayTreeNode, unsigned int index) {
    return arrayTreeNode.getChild(index)->as<babelwires::ValueTreeNode>();
}

babelwires::ValueTreeNode& babelwires::InstanceUtils::getChild(babelwires::ValueTreeNode& arrayTreeNode, unsigned int index) {
    return arrayTreeNode.getChild(index)->as<babelwires::ValueTreeNode>();
}
