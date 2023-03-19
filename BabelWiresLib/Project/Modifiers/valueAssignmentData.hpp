/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

namespace babelwires {
    class Value;

    /// Data used to assign an SimpleValueFeature within a container to a certain value.
    class ValueAssignmentData : public LocalModifierData {
      public:
        CLONEABLE(ValueAssignmentData);
        SERIALIZABLE(ValueAssignmentData, "assign", LocalModifierData, 1);
        
        ValueAssignmentData();
        ValueAssignmentData(std::unique_ptr<Value> value);

        virtual void apply(Feature* targetFeature) const override;
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

      private:
        std::shared_ptr<const Value> m_value;
    };

} // namespace babelwires
