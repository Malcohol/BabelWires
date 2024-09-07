/**
 * SelectRecordVariantModifierData is used to select the variant of a RecordWithVariantsType.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

namespace babelwires {
    /// SelectRecordVariantModifierData is used to select the variant of a RecordWithVariantsType.
    struct SelectRecordVariantModifierData : LocalModifierData {
        virtual void apply(Feature* targetFeature) const override;
        CLONEABLE(SelectRecordVariantModifierData);
        SERIALIZABLE(SelectRecordVariantModifierData, "selectVariant", LocalModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        ShortId m_tagToSelect;
    };
}
