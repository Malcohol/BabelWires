/**
 * SelectUnionBranchModifierData is used to select a set of optionals in a RecordWithOptionalsFeature
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/selectUnionBranchModifierData.hpp>

#include <BabelWiresLib/Features/unionFeature.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/valueFeature.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

void babelwires::SelectUnionBranchModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValue("select", m_tagToSelect);
}

void babelwires::SelectUnionBranchModifierData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    deserializer.deserializeValue("select", m_tagToSelect);
}

void babelwires::SelectUnionBranchModifierData::apply(Feature* targetFeature) const {
    if (UnionFeature* unionFeature = targetFeature->as<UnionFeature>()) {
        unionFeature->selectTag(m_tagToSelect);
        return;
    } else if (auto valueFeature = targetFeature->as<ValueFeature>()) {
        if (auto recordType = valueFeature->getType().as<RecordWithVariantsType>()) {
            const TypeSystem& typeSystem = RootFeature::getTypeSystemAt(*valueFeature);
            ValueHolder newValue = valueFeature->getValue();
            recordType->selectTag(typeSystem, newValue, m_tagToSelect);
            valueFeature->setValue(newValue);
            return;
        }
    }
    throw ModelException() << "Select variant modifier applied to feature which does not have variants";
}

void babelwires::SelectUnionBranchModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    visitor(m_tagToSelect);
}
