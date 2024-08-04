/**
 * A convenient base class for most processors.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Processors/processorFactory.hpp>

namespace babelwires {
    struct ProjectContext;
    class RootFeature;

    /// A convenient base class for most processors which provides a default constructed recordFeature for input and
    /// output features.
    class CommonProcessor : public Processor {
      public:
        CommonProcessor(const ProjectContext& projectContext);

        virtual babelwires::Feature* getInputFeature() override;
        virtual babelwires::Feature* getOutputFeature() override;

        babelwires::RootFeature* getInputRootFeature() { return m_inputFeature.get(); }
        babelwires::RootFeature* getOutputRootFeature() { return m_outputFeature.get(); }

      protected:
        std::unique_ptr<babelwires::RootFeature> m_inputFeature;
        std::unique_ptr<babelwires::RootFeature> m_outputFeature;
    };
} // namespace babelwires
