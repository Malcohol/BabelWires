/**
 * NodeDatas carry the data sufficient to reconstruct a Node.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Modifiers/modifierData.hpp>
#include <BabelWiresLib/Project/projectIds.hpp>
#include <BabelWiresLib/Project/uiPosition.hpp>

#include <Common/Cloning/cloneable.hpp>
#include <Common/Serialization/serializable.hpp>
#include <Common/types.hpp>
#include <Common/Log/userLogger.hpp>

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

namespace babelwires {

    struct UserLogger;
    struct ProjectContext;
    class Node;
    class UntypedRegistry;

    /// Use the custom clone framework to do a shallow clone.
    struct ShallowCloneContext {};

    /// Data of interest only to the UI, and has no bearing on the evaluation logic.
    /// Note: The expand/collapse logic could have been included here, but I decided to pack
    /// it into one EditTree with the modifiers.
    struct UiData : Serializable {
        SERIALIZABLE(UiData, "uiData", void, 1);
        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        /// The position this element is at in the UI.
        UiPosition m_uiPosition;

        /// The size of the element in the UI.
        UiSize m_uiSize;
    };

    /// NodeDatas carry the data sufficient to reconstruct a Node.
    /// The class supports clone(), which returns a deep clone of the object,
    /// and customClone() which returns a clone with the array members unset.
    struct NodeData : Cloneable, CustomCloneable<ShallowCloneContext>, Serializable, ProjectVisitable {
        CLONEABLE_ABSTRACT(NodeData);
        CUSTOM_CLONEABLE_ABSTRACT(NodeData);
        SERIALIZABLE_ABSTRACT(NodeData, void);
        DOWNCASTABLE_TYPE_HIERARCHY(NodeData);

        NodeData() = default;

        /// The copy constructor constructs an array of _clones_ of the modifiers.
        NodeData(const NodeData& other);

        /// Like the copy constructor, but don't copy the modifiers and expanded paths.
        NodeData(const NodeData& other, ShallowCloneContext);

        /// Unless m_factoryVersion is 0 (which means "ignore versions"), this should return false
        /// if the m_factoryVersion is different than the factory version registered in the context.
        /// Warn if it is less and error if it is more.
        virtual bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) = 0;

        std::unique_ptr<Node> createNode(const ProjectContext& context, UserLogger& userLogger,
                                                             NodeId newId) const;

        /// Call the visitor on all the FieldIdentifiers in the element.
        void visitIdentifiers(IdentifierVisitor& visitor) override;

        /// Call the visitor on all the FilePaths in the element.
        /// This base implementation does nothing.
        void visitFilePaths(FilePathVisitor& visitor) override;

        /// The factory which creates Nodes corresponding to this data.
        LongId m_factoryIdentifier = "nofact";

        /// The version of the factory to which these contents relate.
        /// 0 is treated specially, and means that versioning should be ignored, and the data should
        /// be considered implicitly up-to-date.
        VersionNumber m_factoryVersion = 0;

        /// Uniquely identifies the Element within the project.
        NodeId m_id = INVALID_NODE_ID;

        /// Data solely relevant to the UI.
        UiData m_uiData;

        /// The set of modifiers applied to this modifier.
        std::vector<std::unique_ptr<ModifierData>> m_modifiers;

        /// Those paths within the feature which are expanded.
        /// Note: Only those which are present in the features are maintained on serialization.
        std::vector<Path> m_expandedPaths;

      protected:
        virtual std::unique_ptr<Node>
        doCreateNode(const ProjectContext& context, UserLogger& userLogger, NodeId newId) const = 0;

        /// For use when serializing subclasses.
        void addCommonKeyValuePairs(Serializer& serializer) const;
        void getCommonKeyValuePairs(Deserializer& deserializer);

        /// For use when serializing subclasses.
        void serializeModifiers(Serializer& serializer) const;
        void deserializeModifiers(Deserializer& serializer);

        /// For use when serializing subclasses.
        void serializeUiData(Serializer& serializer) const;
        void deserializeUiData(Deserializer& deserializer);

        /// Look up the ID in the reg, and issue a warning if there are version incompatibilities.
        template<typename REGISTRY>
        bool checkFactoryVersionCommon(const REGISTRY& reg, UserLogger& userLogger,
                                   LongId factoryIdentifier, VersionNumber& thisVersion);
    };
} // namespace babelwires

#include <BabelWiresLib/Project/Nodes/nodeData_inl.hpp>
