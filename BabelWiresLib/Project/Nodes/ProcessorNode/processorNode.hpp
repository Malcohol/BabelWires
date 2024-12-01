/**
 * ProcessorNode are Nodes which carry a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/Nodes/node.hpp>

namespace babelwires {
    struct UserLogger;
    struct ProjectContext;
    struct ProcessorNodeData;
    class Processor;
    class ValueTreeRoot;

    class ProcessorNode : public Node {
      public:
        ProcessorNode(const ProjectContext& context, UserLogger& userLogger, const ProcessorNodeData& data,
                         ElementId newId);
        ~ProcessorNode();

        /// Down-cast version of the parent's method.
        const ProcessorNodeData& getElementData() const;

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
