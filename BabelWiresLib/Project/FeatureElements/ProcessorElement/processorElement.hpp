/**
 * ProcessorElement are FeatureElements which carry a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/FeatureElements/node.hpp>

namespace babelwires {
    struct UserLogger;
    struct ProjectContext;
    struct ProcessorElementData;
    class Processor;
    class ValueTreeRoot;

    class ProcessorElement : public Node {
      public:
        ProcessorElement(const ProjectContext& context, UserLogger& userLogger, const ProcessorElementData& data,
                         ElementId newId);
        ~ProcessorElement();

        /// Down-cast version of the parent's method.
        const ProcessorElementData& getElementData() const;

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
