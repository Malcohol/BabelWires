/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWiresLib/Project/Modifiers/modifierData.hpp"

#include "BabelWiresLib/Features/Utilities/modelUtilities.hpp"
#include "BabelWiresLib/Features/numericFeature.hpp"
#include "BabelWiresLib/Features/stringFeature.hpp"
#include "BabelWiresLib/Features/enumFeature.hpp"
#include "BabelWiresLib/FileFormat/fileFeature.hpp"
#include "BabelWiresLib/Project/FeatureElements/featureElement.hpp"
#include "BabelWiresLib/Project/Modifiers/connectionModifier.hpp"
#include "BabelWiresLib/Project/Modifiers/localModifier.hpp"
#include "BabelWiresLib/Project/project.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

babelwires::Feature* babelwires::ModifierData::getTargetFeature(Feature* container) const {
    return &m_pathToFeature.follow(*container);
}

void babelwires::ModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    for (auto& s : m_pathToFeature) {
        if (s.isField()) {
            visitor(s.getField());
        }
    }
}

void babelwires::ModifierData::visitFilePaths(FilePathVisitor& visitor) {
}

std::unique_ptr<babelwires::Modifier> babelwires::LocalModifierData::createModifier() const {
    return std::make_unique<babelwires::LocalModifier>(clone());
}

void babelwires::IntValueAssignmentData::apply(Feature* targetFeature) const {
    if (IntFeature* intFeature = targetFeature->as<IntFeature>()) {
        intFeature->set(m_value);
    } else {
        throw babelwires::ModelException() << "Could not assign an int to a non-int feature";
    }
}

void babelwires::IntValueAssignmentData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValue("value", m_value);
}

void babelwires::IntValueAssignmentData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    deserializer.deserializeValue("value", m_value);
}

void babelwires::RationalValueAssignmentData::apply(Feature* targetFeature) const {
    if (RationalFeature* rationalFeature = targetFeature->as<RationalFeature>()) {
        rationalFeature->set(m_value);
    } else {
        throw babelwires::ModelException() << "Could not assign a rational to a non-rational feature";
    }
}

void babelwires::RationalValueAssignmentData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValue("value", m_value);
}

void babelwires::RationalValueAssignmentData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    deserializer.deserializeValue("value", m_value);
}

void babelwires::StringValueAssignmentData::apply(Feature* targetFeature) const {
    if (StringFeature* stringFeature = targetFeature->as<StringFeature>()) {
        stringFeature->set(m_value);
    } else {
        throw babelwires::ModelException() << "Could not assign a string to a non-string feature";
    }
}

void babelwires::StringValueAssignmentData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValue("value", m_value);
}

void babelwires::StringValueAssignmentData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    deserializer.deserializeValue("value", m_value);
}

void babelwires::EnumValueAssignmentData::apply(Feature* targetFeature) const {
    if (EnumFeature* enumFeature = targetFeature->as<EnumFeature>()) {
        enumFeature->set(m_value);
    } else {
        throw babelwires::ModelException() << "Could not assign an enum value to a non-enum feature";
    }
}

void babelwires::EnumValueAssignmentData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValue("value", m_value);
}

void babelwires::EnumValueAssignmentData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    deserializer.deserializeValue("value", m_value);
}

void babelwires::EnumValueAssignmentData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    visitor(m_value);
}
