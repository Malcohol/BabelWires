/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

namespace babelwires {
    class Value;

    /// Data used to assign a SimpleValueFeature within a container to a certain value.
    class ValueAssignmentData : public LocalModifierData {
      public:
        CLONEABLE(ValueAssignmentData);
        SERIALIZABLE(ValueAssignmentData, "assign", LocalModifierData, 1);
        
        //TODO These should take the target path.
        ValueAssignmentData();
        ValueAssignmentData(std::unique_ptr<EditableValue> value);
        ValueAssignmentData(EditableValueHolder value);

        virtual void apply(ValueTreeNode* target) const override;
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

        EditableValueHolder getValue() const;

      private:
        EditableValueHolder m_value;
    };

} // namespace babelwires
