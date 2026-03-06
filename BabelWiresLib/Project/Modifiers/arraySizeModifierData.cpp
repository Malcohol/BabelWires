/**
 * ArraySizeModifierData sets an array to a certain size.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>

#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <BaseLib/Result/resultDSL.hpp>
#include <BaseLib/Serialization/deserializer.hpp>
#include <BaseLib/Serialization/serializer.hpp>

babelwires::Result babelwires::ArraySizeModifierData::apply(ValueTreeNode* target) const {
    if (const ArrayType* arrayType = target->getType()->tryAs<ArrayType>()) {
        const TypeSystem& typeSystem = target->getTypeSystem();
        ValueHolder newValue = target->getValue();
        DO_OR_ERROR(arrayType->setSize(typeSystem, newValue, m_size));
        target->setValue(newValue);
        return {};
    }

    return babelwires::Error() << "Cannot initialize size of non-array";
}

std::unique_ptr<babelwires::Modifier> babelwires::ArraySizeModifierData::createModifier() const {
    return std::make_unique<babelwires::ArraySizeModifier>(clone());
}

void babelwires::ArraySizeModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_targetPath);
    serializer.serializeValue("size", m_size);
}

babelwires::Result babelwires::ArraySizeModifierData::deserializeContents(Deserializer& deserializer) {
    DO_OR_ERROR(deserializer.deserializeValue("path", m_targetPath));
    DO_OR_ERROR(deserializer.deserializeValue("size", m_size));
    return {};
}

babelwires::Result babelwires::ArraySizeModifierData::addEntries(ValueTreeNode* target, int indexOfNewElement,
                                                     int numEntriesToAdd) {
    assert((numEntriesToAdd > 0) && "numEntriesToAdd must be strictly positive");
    m_size += numEntriesToAdd;

    if (const ArrayType* arrayType = target->getType()->tryAs<ArrayType>()) {
        const TypeSystem& typeSystem = target->getTypeSystem();
        ValueHolder newValue = target->getValue();
        DO_OR_ERROR(arrayType->insertEntries(typeSystem, newValue, indexOfNewElement, numEntriesToAdd));
        target->setValue(newValue);
        return {};
    }

    return babelwires::Error() << "Cannot resize non-array";
}

babelwires::Result babelwires::ArraySizeModifierData::removeEntries(ValueTreeNode* target, int indexOfElementToRemove,
                                                        int numEntriesToRemove) {
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
    assert((m_size >= numEntriesToRemove) && "You can't have ArraySizeModifierData with negative size");
    m_size -= numEntriesToRemove;
    
    if (const ArrayType* arrayType = target->getType()->tryAs<ArrayType>()) {
        ValueHolder newValue = target->getValue();
        DO_OR_ERROR(arrayType->removeEntries(newValue, indexOfElementToRemove, numEntriesToRemove));
        target->setValue(newValue);
        return {};
    }

    return babelwires::Error() << "Cannot resize non-array";
}
