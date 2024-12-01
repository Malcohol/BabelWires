/**
 * ValueElementData describes the construction of a ValueElement.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once


#include <BabelWiresLib/Project/FeatureElements/nodeData.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

namespace babelwires {
    /// Describes the construction of a ValueElement.
    class ValueElementData : public NodeData {
      public:
        CLONEABLE(ValueElementData);
        CUSTOM_CLONEABLE(ValueElementData);
        SERIALIZABLE(ValueElementData, "value", NodeData, 1);
        ValueElementData() = default;
        ValueElementData(TypeRef typeRef);
        ValueElementData(const ValueElementData& other) = default;
        ValueElementData(const ValueElementData& other, ShallowCloneContext);

        bool checkFactoryVersion(const ProjectContext& context, UserLogger& userLogger) override;

        void serializeContents(Serializer& serializer) const override;
        void deserializeContents(Deserializer& deserializer) override;

        const TypeRef& getTypeRef() const;

      protected:
        std::unique_ptr<Node> doCreateFeatureElement(const ProjectContext& context, UserLogger& userLogger,
                                                               ElementId newId) const override;

      private:
        TypeRef m_typeRef;
    };
}
