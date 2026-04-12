/**
 * ProcessorNode are Nodes which carry a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Project/Nodes/node.hpp>

namespace babelwires {
    struct UserLogger;
    struct Context;
    struct ProcessorNodeData;
    class Processor;
    class ValueTreeRoot;

    class BABELWIRESLIB_API ProcessorNode : public Node {
      public:
        DOWNCASTABLE(ProcessorNode, Node);
        ProcessorNode(const Context& context, UserLogger& userLogger, const ProcessorNodeData& data,
                         NodeId newId);
        ~ProcessorNode();

        /// Down-cast version of the parent's method.
        const ProcessorNodeData& getNodeData() const;

        virtual const ValueTreeNode* getInput() const override;
        virtual const ValueTreeNode* getOutput() const override;

      protected:
        ValueTreeNode* doGetInputNonConst() override;
        ValueTreeNode* doGetOutputNonConst() override;
        void doProcess(UserLogger& userLogger) override;

      protected:
        std::string getRootLabel() const;
        void setProcessor(std::unique_ptr<Processor> processor);

      private:
        std::unique_ptr<Processor> m_processor;

        /// Non-null when the defined processor could not be constructed.
        std::unique_ptr<babelwires::ValueTreeRoot> m_failedValueTree;
    };

} // namespace babelwires
