/**
 * ConnectionModifierData used to assign a Feature within a container to a value from another element.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/feature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

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
    if (!(applyEvenIfSourceUnchanged || sourceFeature->isChanged(Feature::Changes::SomethingChanged))) {
        return;
    }

    targetFeature->assign(*sourceFeature);
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

void babelwires::ConnectionModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    for (auto& s : m_pathToSourceFeature) {
        if (s.isField()) {
            visitor(s.getField());
        }
    }
}
