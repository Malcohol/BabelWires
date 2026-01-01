/**
 * ValueNodeData describes the construction of a ValueNode.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once


#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>

namespace babelwires {
    /// Describes the construction of a ValueNode.
    class ValueNodeData : public NodeData {
      public:
        CLONEABLE(ValueNodeData);
        CUSTOM_CLONEABLE(ValueNodeData);
        SERIALIZABLE(ValueNodeData, "value", NodeData, 1);
        ValueNodeData() = default;
        ValueNodeData(TypeExp typeExp);
        ValueNodeData(const ValueNodeData& other) = default;
        ValueNodeData(const ValueNodeData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        const TypeExp& getTypeExp() const;

      protected:
        std::unique_ptr<Node> doCreateNode(const ProjectContext& context, UserLogger& userLogger,
                                                               NodeId newId) const override;

      private:
        TypeExp m_typeExp;
    };
}
