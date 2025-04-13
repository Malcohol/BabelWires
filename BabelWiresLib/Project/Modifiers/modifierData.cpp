/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/Project/project.hpp>
#include <BabelWiresLib/ValueTree/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/ValueTree/valueTreeNode.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

babelwires::ValueTreeNode* babelwires::ModifierData::getTarget(ValueTreeNode* container) const {
    return &followPath(m_targetPath, *container);
}

void babelwires::ModifierData::visitIdentifiers(IdentifierVisitor& visitor) {
    m_targetPath.visitIdentifiers(visitor);
}

void babelwires::ModifierData::visitFilePaths(FilePathVisitor& visitor) {}

std::unique_ptr<babelwires::Modifier> babelwires::LocalModifierData::createModifier() const {
    return std::make_unique<babelwires::LocalModifier>(clone());
}
