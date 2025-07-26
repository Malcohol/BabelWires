/**
 * SetTypeVariableModifierData is used to set a type variable to specific type.
 *
 * (C) 2025 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

namespace babelwires {
    /// SetTypeVariableModifierData is used to select the variant of a RecordWithVariantsType.
    struct SetTypeVariableModifierData : LocalModifierData {
        CLONEABLE(SetTypeVariableModifierData);
        SERIALIZABLE(SetTypeVariableModifierData, "setTypeVariable", LocalModifierData, 1);
        virtual void apply(ValueTreeNode* target) const override;
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        /// The type to which the variable should be set.
        TypeRef m_typeRef;

        /// The index of the variable in the generic type.
        unsigned int m_variableIndex;
    };
}
