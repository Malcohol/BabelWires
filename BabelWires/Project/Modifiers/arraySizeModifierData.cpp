#include "BabelWires/Project/Modifiers/arraySizeModifierData.hpp"

#include "BabelWires/Features/arrayFeature.hpp"
#include "BabelWires/Project/Modifiers/localModifier.hpp"
#include "BabelWires/Project/project.hpp"
#include "BabelWires/Project/Modifiers/arraySizeModifier.hpp"
#include "BabelWires/Features/modelExceptions.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

void babelwires::ArrayInitializationData::apply(Feature* targetFeature) const {
    if (ArrayFeature* array = dynamic_cast<ArrayFeature*>(targetFeature)) {
        array->setSize(m_size);
    } else {
        throw babelwires::ModelException() << "Cannot initialize size of non-array";
    }
}

std::unique_ptr<babelwires::Modifier> babelwires::ArrayInitializationData::createModifier() const {
    return std::make_unique<babelwires::ArraySizeModifier>(clone());
}

void babelwires::ArrayInitializationData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValue("size", m_size);
}

void babelwires::ArrayInitializationData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    deserializer.deserializeValue("size", m_size);
}

void babelwires::ArrayInitializationData::addEntries(Feature* targetFeature, int indexOfNewElement,
                                                     int numEntriesToAdd) {
    assert((numEntriesToAdd > 0) && "numEntriesToAdd must be strictly positive");
    m_size += numEntriesToAdd;
    if (ArrayFeature* array = dynamic_cast<ArrayFeature*>(targetFeature)) {
        for (int i = 0; i < numEntriesToAdd; ++i) {
            array->addEntry(indexOfNewElement);
        }
    } else {
        throw babelwires::ModelException() << "Cannot resize non-array";
    }
}

void babelwires::ArrayInitializationData::removeEntries(Feature* targetFeature, int indexOfElementToRemove,
                                                        int numEntriesToRemove) {
    assert((numEntriesToRemove > 0) && "numEntriesToRemove must be strictly positive");
    assert((m_size >= numEntriesToRemove) && "You can't have ArrayInitializationData with negative size");
    m_size -= numEntriesToRemove;
    if (ArrayFeature* array = dynamic_cast<ArrayFeature*>(targetFeature)) {
        for (int i = 0; i < numEntriesToRemove; ++i) {
            array->removeEntry(indexOfElementToRemove);
        }
    } else {
        throw babelwires::ModelException() << "Cannot resize non-array";
    }
}
