/**
 * ArraySizeModifierData sets an array to a certain size.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp"

#include "BabelWiresLib/Features/arrayFeature.hpp"
#include "BabelWiresLib/Project/Modifiers/localModifier.hpp"
#include "BabelWiresLib/Project/project.hpp"
#include "BabelWiresLib/Project/Modifiers/arraySizeModifier.hpp"
#include "BabelWiresLib/Features/modelExceptions.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

void babelwires::ArraySizeModifierData::apply(Feature* targetFeature) const {
    if (ArrayFeature* array = targetFeature->as<ArrayFeature>()) {
        array->setSize(m_size);
    } else {
        throw babelwires::ModelException() << "Cannot initialize size of non-array";
    }
}

std::unique_ptr<babelwires::Modifier> babelwires::ArraySizeModifierData::createModifier() const {
    return std::make_unique<babelwires::ArraySizeModifier>(clone());
}

void babelwires::ArraySizeModifierData::serializeContents(Serializer& serializer) const {
    ModifierData::serializeContents(serializer);
    serializer.serializeValue("size", m_size);
}

babelwires::ArraySizeModifierData::ArraySizeModifierData(Deserializer& deserializer) : LocalModifierData(deserializer) {
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
    } else {
        throw babelwires::ModelException() << "Cannot resize non-array";
    }
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
    } else {
        throw babelwires::ModelException() << "Cannot resize non-array";
    }
}
