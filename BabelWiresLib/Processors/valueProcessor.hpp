/**
 *
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Processors/commonProcessor.hpp>
#include <BabelWiresLib/TypeSystem/valueHolder.hpp>

#include <memory>

namespace babelwires {
    struct UserLogger;
    class RootFeature;
    class TypeRef;
    class ProjectContext;
    class ValueFeature;
    template <typename VALUE_HOLDER, typename TYPE> class Instance;

    class ValueProcessor : public CommonProcessor {
      public:
        ValueProcessor(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
                           const TypeRef& outputTypeRef);

      protected:
        /// Note: Implementations do not need to worry about backing-up or resolving changes in the output feature.
        virtual void processValue(UserLogger& userLogger, const ValueFeature& inputFeature, ValueFeature& outputFeature) const = 0;

        /// The root feature has a single step to the value feature, which always uses this identifier.
        static ShortId getStepToValue();

      public:
        void process(UserLogger& userLogger) override;
    };
} // namespace babelwires
