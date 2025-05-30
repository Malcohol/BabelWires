/**
 * ConnectionModifierData used to assign a value within a container to a value from another node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>

namespace babelwires {
    /// Data used to assign a value within a container to a value from another node.
    struct ConnectionModifierData : ModifierData {
        /// Find the source ValueTreeNode in the project, or throw.
        const ValueTreeNode* getSourceTreeNode(const Project& project) const;

        /// Apply the source value to the target ValueTreeNode.
        void apply(const ValueTreeNode* source, ValueTreeNode* target, bool applyEvenIfSourceUnchanged) const;

        std::unique_ptr<Modifier> createModifier() const override;

        CLONEABLE(ConnectionModifierData);
        SERIALIZABLE(ConnectionModifierData, "assignFrom", ModifierData, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        NodeId m_sourceId;
        Path m_sourcePath;
    };
}
