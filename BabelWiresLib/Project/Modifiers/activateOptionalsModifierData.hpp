/**
 * ActivateOptionalsModifierData is used to select a set of optionals in a RecordWithOptionalsFeature
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"

namespace babelwires {
    /// Data used to select a set of optionals in a RecordWithOptionalsFeature
    struct ActivateOptionalsModifierData : LocalModifierData {
        virtual void apply(Feature* targetFeature) const override;
        CLONEABLE(ActivateOptionalsModifierData);
        SERIALIZABLE(ActivateOptionalsModifierData, "activatedOptionals", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitFields(FieldVisitor& visitor) override;

        std::vector<FieldIdentifier> m_selectedOptionals;
    };
}
