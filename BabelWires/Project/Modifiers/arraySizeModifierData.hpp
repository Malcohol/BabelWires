/**
 * ArrayInitializationData sets an array to a certain size.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/Modifiers/modifierData.hpp"

namespace babelwires {
    /// Data used to set an array within a container to a certain size.
    struct ArrayInitializationData : LocalModifierData {
        void apply(Feature* targetFeature) const override;
        std::unique_ptr<Modifier> createModifier() const override;

        void addEntries(Feature* targetFeature, int indexOfNewElement, int numEntriesToAdd);
        void removeEntries(Feature* targetFeature, int indexOfElementToRemove, int numEntriesToRemove);
        CLONEABLE(ArrayInitializationData);

        SERIALIZABLE(ArrayInitializationData, "arraySize", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        int m_size = 0;
    };
}
