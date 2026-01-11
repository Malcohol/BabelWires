/**
 * SelectRecordVariantModifierData is used to select the variant of a RecordWithVariantsType.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/selectRecordVariantModifierData.hpp>

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

void babelwires::SelectRecordVariantModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_targetPath);
    serializer.serializeValue("select", m_tagToSelect);
}

void babelwires::SelectRecordVariantModifierData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_targetPath);
    deserializer.deserializeValue("select", m_tagToSelect);
}

void babelwires::SelectRecordVariantModifierData::apply(ValueTreeNode* target) const {
    if (auto recordType = target->getType()->tryAs<RecordWithVariantsType>()) {
        const TypeSystem& typeSystem = target->getTypeSystem();
        ValueHolder newValue = target->getValue();
        recordType->selectTag(typeSystem, newValue, m_tagToSelect);
        target->setValue(newValue);
        return;
    }
    throw ModelException() << "Select variant modifier applied to ValueTreeNode which does not have variants";
}

void babelwires::SelectRecordVariantModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    visitor(m_tagToSelect);
}
