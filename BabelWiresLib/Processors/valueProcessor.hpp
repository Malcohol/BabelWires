/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>
#include <BabelWiresLib/Processors/processor.hpp>

#include <memory>

namespace babelwires {
    struct UserLogger;
    class TypeRef;
    class ProjectContext;
    class ValueFeature;
    class SimpleValueFeature;

    class ValueProcessor : public Processor {
      public:
        ValueProcessor(const ProjectContext& projectContext, const TypeRef& inputTypeRef, const TypeRef& outputTypeRef);

      protected:
        /// Note: Implementations do not need to worry about backing-up or resolving changes in the output feature.
        virtual void processValue(UserLogger& userLogger, const ValueFeature& inputFeature,
                                  ValueFeature& outputFeature) const = 0;
      public:
        ValueFeature* getInputFeature() override;
        ValueFeature* getOutputFeature() override;
        void process(UserLogger& userLogger) override final;

      protected:
        std::unique_ptr<SimpleValueFeature> m_inputFeature;
        std::unique_ptr<SimpleValueFeature> m_outputFeature;
    };
} // namespace babelwires
