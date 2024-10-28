/**
 * ActivateOptionalsModifierData is used to select a set of optionals in a RecordType
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

namespace babelwires {
    /// ActivateOptionalsModifierData is used to select a set of optionals in a RecordType
    struct ActivateOptionalsModifierData : LocalModifierData {
        virtual void apply(ValueTreeNode* target) const override;
        CLONEABLE(ActivateOptionalsModifierData);
        SERIALIZABLE(ActivateOptionalsModifierData, "activatedOptionals", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        std::vector<ShortId> m_selectedOptionals;
    };
}
