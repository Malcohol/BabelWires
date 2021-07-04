/**
 * ProcessorElement are FeatureElements which carry a processor.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWires/Project/FeatureElements/featureElement.hpp"

namespace babelwires {
    struct UserLogger;
    struct ProjectContext;
    struct ProcessorElementData;
    class Processor;

    class ProcessorElement : public FeatureElement {
      public:
        ProcessorElement(const ProjectContext& context, UserLogger& userLogger, const ProcessorElementData& data,
                         ElementId newId);

        /// Down-cast version of the parent's method.
        const ProcessorElementData& getElementData() const;

        virtual RecordFeature* getInputFeature() override;
        virtual RecordFeature* getOutputFeature() override;

      protected:
        void setProcessor(std::unique_ptr<Processor> processor);
        virtual void doProcess(UserLogger& userLogger) override;

      private:
        std::unique_ptr<Processor> m_processor;

        /// Non-null when the defined processor could not be constructed.
        std::unique_ptr<babelwires::RecordFeature> m_sharedDummyFeature;
    };

} // namespace babelwires
