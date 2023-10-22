/**
 * ArraySizeModifierData sets an array to a certain size.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

void babelwires::ArraySizeModifierData::apply(Feature* targetFeature) const {
    if (ArrayFeature* array = targetFeature->as<ArrayFeature>()) {
        array->setSize(m_size);
        return;
    }

    if (ValueFeature* value = targetFeature->as<ValueFeature>()) {
        if (const ArrayType* arrayType = value->getType().as<ArrayType>()) {
            const ProjectContext& context = RootFeature::getProjectContextAt(*value);
            ValueHolder newValue = value->getValue();
            arrayType->setSize(context.m_typeSystem, newValue, m_size);
            value->setValue(newValue);
            return;
        }
    }

    throw babelwires::ModelException() << "Cannot initialize size of non-array";
}

std::unique_ptr<babelwires::Modifier> babelwires::ArraySizeModifierData::createModifier() const {
    return std::make_unique<babelwires::ArraySizeModifier>(clone());
}

void babelwires::ArraySizeModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValue("size", m_size);
}

void babelwires::ArraySizeModifierData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    deserializer.deserializeValue("size", m_size);
}

void babelwires::ArraySizeModifierData::addEntries(Feature* targetFeature, int indexOfNewElement,
                                                     int numEntriesToAdd) {
    assert((numEntriesToAdd > 0) && "numEntriesToAdd must be strictly positive");
    m_size += numEntriesToAdd;
    if (ArrayFeature* array = targetFeature->as<ArrayFeature>()) {
        for (int i = 0; i < numEntriesToAdd; ++i) {
            array->addEntry(indexOfNewElement);
        }
        return;
    } 
    
    if (ValueFeature* value = targetFeature->as<ValueFeature>()) {
        if (const ArrayType* arrayType = value->getType().as<ArrayType>()) {
            const ProjectContext& context = RootFeature::getProjectContextAt(*value);
            ValueHolder newValue = value->getValue();
            arrayType->insertEntries(context.m_typeSystem, newValue, indexOfNewElement, numEntriesToAdd);
            value->setValue(newValue);
            return;
        }
    }

    throw babelwires::ModelException() << "Cannot resize non-array";
}

void babelwires::ArraySizeModifierData::removeEntries(Feature* targetFeature, int indexOfElementToRemove,
                                                        int numEntriesToRemove) {
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
    assert((m_size >= numEntriesToRemove) && "You can't have ArraySizeModifierData with negative size");
    m_size -= numEntriesToRemove;
    if (ArrayFeature* array = targetFeature->as<ArrayFeature>()) {
        for (int i = 0; i < numEntriesToRemove; ++i) {
            array->removeEntry(indexOfElementToRemove);
        }
        return;
    }
    
    if (ValueFeature* value = targetFeature->as<ValueFeature>()) {
        if (const ArrayType* arrayType = value->getType().as<ArrayType>()) {
            const ProjectContext& context = RootFeature::getProjectContextAt(*value);
            ValueHolder newValue = value->getValue();
            arrayType->removeEntries(newValue, indexOfElementToRemove, numEntriesToRemove);
            value->setValue(newValue);
            return;
        }
    }

    throw babelwires::ModelException() << "Cannot resize non-array";
}
