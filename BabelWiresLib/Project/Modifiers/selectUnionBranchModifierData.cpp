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
    UnionFeature* unionFeature = targetFeature->as<UnionFeature>();
    if (!unionFeature) {
        throw ModelException() << "Union modifier applied to feature which is not a union";
    }
    unionFeature->selectTag(m_tagToSelect);
}

void babelwires::SelectUnionBranchModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    visitor(m_tagToSelect);
}
