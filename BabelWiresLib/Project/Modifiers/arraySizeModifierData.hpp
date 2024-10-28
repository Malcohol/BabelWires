/**
 * ArraySizeModifierData sets an array to a certain size.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

namespace babelwires {
    /// Data used to set an array within a container to a certain size.
    struct ArraySizeModifierData : LocalModifierData {
        void apply(ValueTreeNode* target) const override;
        std::unique_ptr<Modifier> createModifier() const override;

        void addEntries(ValueTreeNode* target, int indexOfNewElement, int numEntriesToAdd);
        void removeEntries(ValueTreeNode* target, int indexOfElementToRemove, int numEntriesToRemove);
        CLONEABLE(ArraySizeModifierData);

        SERIALIZABLE(ArraySizeModifierData, "arraySize", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        int m_size = 0;
    };
}
