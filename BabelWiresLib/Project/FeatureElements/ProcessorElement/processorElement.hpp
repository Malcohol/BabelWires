/**
 * ProcessorElement are FeatureElements which carry a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Project/FeatureElements/featureElement.hpp>

namespace babelwires {
    struct UserLogger;
    struct ProjectContext;
    struct ProcessorElementData;
    class Processor;
    class SimpleValueFeature;

    class ProcessorElement : public FeatureElement {
      public:
        ProcessorElement(const ProjectContext& context, UserLogger& userLogger, const ProcessorElementData& data,
                         ElementId newId);
        ~ProcessorElement();

        /// Down-cast version of the parent's method.
        const ProcessorElementData& getElementData() const;

        virtual const Feature* getInputFeature() const override;
        virtual const Feature* getOutputFeature() const override;

      protected:
        Feature* doGetInputFeatureNonConst() override;
        Feature* doGetOutputFeatureNonConst() override;
        void doProcess(UserLogger& userLogger) override;

      protected:
        std::string getRootLabel() const;
        void setProcessor(std::unique_ptr<Processor> processor);

      private:
        std::unique_ptr<Processor> m_processor;

        /// Non-null when the defined processor could not be constructed.
        std::unique_ptr<babelwires::SimpleValueFeature> m_failedFeature;
    };

} // namespace babelwires
