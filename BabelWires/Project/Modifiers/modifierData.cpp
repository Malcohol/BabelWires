/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/Modifiers/modifierData.hpp"

#include "BabelWires/Features/Utilities/modelUtilities.hpp"
#include "BabelWires/Features/numericFeature.hpp"
#include "BabelWires/Features/stringFeature.hpp"
#include "BabelWires/FileFormat/fileFeature.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/Modifiers/connectionModifier.hpp"
#include "BabelWires/Project/Modifiers/localModifier.hpp"
#include "BabelWires/Project/project.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

babelwires::Feature* babelwires::ModifierData::getTargetFeature(Feature* container) const {
    return &m_pathToFeature.follow(*container);
}

std::unique_ptr<babelwires::Modifier> babelwires::LocalModifierData::createModifier() const {
    return std::make_unique<babelwires::LocalModifier>(clone());
}

void babelwires::IntValueAssignmentData::apply(Feature* targetFeature) const {
    if (IntFeature* intFeature = dynamic_cast<IntFeature*>(targetFeature)) {
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
    if (RationalFeature* rationalFeature = dynamic_cast<RationalFeature*>(targetFeature)) {
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
    if (StringFeature* stringFeature = dynamic_cast<StringFeature*>(targetFeature)) {
        stringFeature->set(m_value);
    } else {
        throw babelwires::ModelException() << "Could not assign a string to a non-string field";
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

const babelwires::Feature* babelwires::AssignFromFeatureData::getSourceFeature(const Project& project) const {
    const FeatureElement* sourceElement = project.getFeatureElement(m_sourceId);
    if (!sourceElement) {
        throw babelwires::ModelException()
            << "Could not find the connection source (element with id=" << m_sourceId << ")";
    }

    /*
    if (sourceElement->isFailed())
    {
            throw babelwires::ModelException() << "The connection source (element with id=" << m_sourceId << ")
    failed.";
    }
    */

    const Feature* const outputFeature = sourceElement->getOutputFeature();
    if (!outputFeature) {
        throw babelwires::ModelException()
            << "The connection source (element with id=" << m_sourceId << ") has no outputs";
    }

    try {
        return &m_pathToSourceFeature.follow(*outputFeature);
    } catch (const std::exception& e) {
        throw babelwires::ModelException()
            << e.what() << "; when looking for source feature in element with id=" << m_sourceId;
    }
}

void babelwires::AssignFromFeatureData::apply(const Feature* sourceFeature, Feature* targetFeature,
                                              bool applyEvenIfSourceUnchanged) const {
    if (!(applyEvenIfSourceUnchanged || sourceFeature->isChanged(Feature::Changes::ValueChanged))) {
        return;
    }

    auto targetValueFeature = dynamic_cast<babelwires::ValueFeature*>(targetFeature);
    if (!targetValueFeature) {
        throw babelwires::ModelException() << "Cannot modify a non-value field";
    }

    auto sourceValueFeature = dynamic_cast<const babelwires::ValueFeature*>(sourceFeature);
    if (!sourceValueFeature) {
        throw babelwires::ModelException()
            << "Cannot apply from the non-value field at " << m_pathToSourceFeature << " in element id=" << m_sourceId;
    }

    targetValueFeature->assign(*sourceValueFeature);
}

void babelwires::AssignFromFeatureData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValue("sourceId", m_sourceId);
    serializer.serializeValue("sourcePath", m_pathToSourceFeature);
}

void babelwires::AssignFromFeatureData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    deserializer.deserializeValue("sourceId", m_sourceId);
    deserializer.deserializeValue("sourcePath", m_pathToSourceFeature);
}

std::unique_ptr<babelwires::Modifier> babelwires::AssignFromFeatureData::createModifier() const {
    return std::make_unique<babelwires::ConnectionModifier>(clone());
}
