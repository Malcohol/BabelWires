/**
 * ValueNodeData describes the construction of a ValueNode.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once


#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

namespace babelwires {
    /// Describes the construction of a ValueNode.
    class ValueNodeData : public NodeData {
      public:
        CLONEABLE(ValueNodeData);
        CUSTOM_CLONEABLE(ValueNodeData);
        SERIALIZABLE(ValueNodeData, "value", NodeData, 1);
        ValueNodeData() = default;
        ValueNodeData(TypeRef typeRef);
        ValueNodeData(const ValueNodeData& other) = default;
        ValueNodeData(const ValueNodeData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        const TypeRef& getTypeRef() const;

      protected:
        std::unique_ptr<Node> doCreateNode(const ProjectContext& context, UserLogger& userLogger,
                                                               ElementId newId) const override;

      private:
        TypeRef m_typeRef;
    };
}
