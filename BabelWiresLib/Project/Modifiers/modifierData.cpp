/**
 * ModifierDatas carry the data sufficient to reconstruct a Modifier.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/FileFormat/fileFeature.hpp>
#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifier.hpp>
#include <BabelWiresLib/Project/Modifiers/localModifier.hpp>
#include <BabelWiresLib/Project/project.hpp>

#include <Common/Serialization/deserializer.hpp>
#include <Common/Serialization/serializer.hpp>

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
