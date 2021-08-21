/**
 * ConnectionModifierData used to assign a ValueFeature within a container to a value from another element.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/Modifiers/connectionModifierData.hpp"

#include "BabelWires/Features/Utilities/modelUtilities.hpp"
#include "BabelWires/Project/FeatureElements/featureElement.hpp"
#include "BabelWires/Project/Modifiers/connectionModifier.hpp"
#include "BabelWires/Project/Modifiers/localModifier.hpp"
#include "BabelWires/Project/project.hpp"
#include "BabelWires/Features/modelExceptions.hpp"
#include "BabelWires/Features/recordFeature.hpp"

#include "Common/Serialization/deserializer.hpp"
#include "Common/Serialization/serializer.hpp"

const babelwires::Feature* babelwires::ConnectionModifierData::getSourceFeature(const Project& project) const {
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

void babelwires::ConnectionModifierData::apply(const Feature* sourceFeature, Feature* targetFeature,
                                              bool applyEvenIfSourceUnchanged) const {
    if (!(applyEvenIfSourceUnchanged || sourceFeature->isChanged(Feature::Changes::ValueChanged))) {
        return;
    }

    auto targetValueFeature = targetFeature->asA<babelwires::ValueFeature>();
    if (!targetValueFeature) {
        throw babelwires::ModelException() << "Cannot modify a non-value field";
    }

    auto sourceValueFeature = sourceFeature->asA<const babelwires::ValueFeature>();
    if (!sourceValueFeature) {
        throw babelwires::ModelException()
            << "Cannot apply from the non-value field at " << m_pathToSourceFeature << " in element id=" << m_sourceId;
    }

    targetValueFeature->assign(*sourceValueFeature);
}

void babelwires::ConnectionModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_pathToFeature);
    serializer.serializeValue("sourceId", m_sourceId);
    serializer.serializeValue("sourcePath", m_pathToSourceFeature);
}

void babelwires::ConnectionModifierData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_pathToFeature);
    deserializer.deserializeValue("sourceId", m_sourceId);
    deserializer.deserializeValue("sourcePath", m_pathToSourceFeature);
}

std::unique_ptr<babelwires::Modifier> babelwires::ConnectionModifierData::createModifier() const {
    return std::make_unique<babelwires::ConnectionModifier>(clone());
}

void babelwires::ConnectionModifierData::visitFields(FieldVisitor& visitor) {
    ModifierData::visitFields(visitor);
    for (auto& s : m_pathToSourceFeature) {
        if (s.isField()) {
            visitor(s.getField());
        }
    }
}
