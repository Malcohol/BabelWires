/**
 * A convenient base class for most processors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Processors/processor.hpp"
#include "BabelWiresLib/Processors/processorFactory.hpp"

namespace babelwires {
    struct ProjectContext;

    /// A convenient base class for most processors which provides a default constructed recordFeature for input and
    /// output features.
    class CommonProcessor : public Processor {
      public:
        CommonProcessor(const ProjectContext& projectContext);

        virtual babelwires::RootFeature* getInputFeature() override;
        virtual babelwires::RootFeature* getOutputFeature() override;

      protected:
        std::unique_ptr<babelwires::RootFeature> m_inputFeature;
        std::unique_ptr<babelwires::RootFeature> m_outputFeature;
    };

    /// A convenient base class for processor factories.
    template <typename PROCESSOR> class CommonProcessorFactory : public ProcessorFactory {
      public:
        CommonProcessorFactory(LongIdentifier identifier, VersionNumber version)
            : ProcessorFactory(identifier, version) {}

        std::unique_ptr<babelwires::Processor> createNewProcessor(const ProjectContext& projectContext) const override {
            return std::make_unique<PROCESSOR>(projectContext);
        }
    };

} // namespace babelwires
