/**
 * SetTypeVariableModifierData is used to assign specific types to type variables.
 *
 * (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>

namespace babelwires {
    /// SetTypeVariableModifierData is used to assign specific types to type variables.
    struct SetTypeVariableModifierData : LocalModifierData {
        CLONEABLE(SetTypeVariableModifierData);
        SERIALIZABLE(SetTypeVariableModifierData, "setTypeVariable", LocalModifierData, 1);
        virtual void apply(ValueTreeNode* target) const override;
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        std::vector<TypeExp> m_typeAssignments;
    };
}
