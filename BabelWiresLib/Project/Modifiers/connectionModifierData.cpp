/**
 * ConnectionModifierData used to assign a value within a container to a value from another node.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>

#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/ValueTree/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

const babelwires::ValueTreeNode* babelwires::ConnectionModifierData::getSourceTreeNode(const Project& project) const {
    const Node* sourceElement = project.getNode(m_sourceId);
    if (!sourceElement) {
        throw babelwires::ModelException()
            << "Could not find the connection source (node with id=" << m_sourceId << ")";
    }

    /*
    if (sourceElement->isFailed())
    {
            throw babelwires::ModelException() << "The connection source (node with id=" << m_sourceId << ")
    failed.";
    }
    */

    const ValueTreeNode* const output = sourceElement->getOutput();
    if (!output) {
        throw babelwires::ModelException()
            << "The connection source (node with id=" << m_sourceId << ") has no outputs";
    }

    try {
        return &followPath(m_sourcePath, *output);
    } catch (const std::exception& e) {
        throw babelwires::ModelException() << e.what() << "; when looking for source in node with id=" << m_sourceId;
    }
}

void babelwires::ConnectionModifierData::apply(const ValueTreeNode* source, ValueTreeNode* target,
                                               bool applyEvenIfSourceUnchanged) const {
    if (!(applyEvenIfSourceUnchanged || source->isChanged(ValueTreeNode::Changes::SomethingChanged))) {
        return;
    }

    target->assign(*source);
}

void babelwires::ConnectionModifierData::serializeContents(Serializer& serializer) const {
    serializer.serializeValue("path", m_targetPath);
    serializer.serializeValue("sourceId", m_sourceId);
    serializer.serializeValue("sourcePath", m_sourcePath);
}

void babelwires::ConnectionModifierData::deserializeContents(Deserializer& deserializer) {
    deserializer.deserializeValue("path", m_targetPath);
    deserializer.deserializeValue("sourceId", m_sourceId);
    deserializer.deserializeValue("sourcePath", m_sourcePath);
}

std::unique_ptr<babelwires::Modifier> babelwires::ConnectionModifierData::createModifier() const {
    return std::make_unique<babelwires::ConnectionModifier>(clone());
}

void babelwires::ConnectionModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    ModifierData::visitIdentifiers(visitor);
    m_sourcePath.visitIdentifiers(visitor);
}
