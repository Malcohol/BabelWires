/**
 * ValueNodes are FeatureElements which carry a value.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/node.hpp>
#include <BabelWiresLib/TypeSystem/typeRef.hpp>

namespace babelwires {
    struct UserLogger;
    class ValueNodeData;
    class ValueTreeRoot;

    class ValueNode : public Node {
      public:
        ValueNode(const ProjectContext& context, UserLogger& userLogger, const ValueNodeData& data,
                         ElementId newId);
        ~ValueNode();

        /// Down-cast version of the parent's method.
        const ValueNodeData& getElementData() const;

        virtual const ValueTreeNode* getInput() const override;
        virtual const ValueTreeNode* getOutput() const override;

      protected:
        ValueTreeNode* doGetInputNonConst() override;
        ValueTreeNode* doGetOutputNonConst() override;
        void doProcess(UserLogger& userLogger) override;

      protected:
        std::string getRootLabel() const;

      private:
        std::unique_ptr<babelwires::ValueTreeRoot> m_valueTreeRoot;
    };

} // namespace babelwires
