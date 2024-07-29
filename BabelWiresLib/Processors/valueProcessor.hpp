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

    class ValueProcessorBase : public CommonProcessor {
      public:
        ValueProcessorBase(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
                           const TypeRef& outputTypeRef);

      protected:
        /// The root feature has a single step to the value feature, which always uses this identifier.
        static ShortId getStepToValue();

        const SimpleValueFeature& getInputValueFeature() const;
        SimpleValueFeature& getOutputValueFeature();
    };

    /*
        class SimpleValueProcessor : public ValueProcessorBase {
          public:
            SimpleValueProcessor(const ProjectContext& projectContext, const TypeRef& inputTypeRef,
                               const TypeRef& outputTypeRef);

          protected:
            virtual void processValue(UserLogger& userLogger, const Type& inputType, const ValueHolder& inputValue,
                                      const Type& outputType, ValueHolder& outputValue) const = 0;

            void process(UserLogger& userLogger) override;
        };
    */

    template <typename INPUT_TYPE, typename OUTPUT_TYPE> class ValueProcessor : public ValueProcessorBase {
      public:
        ValueProcessor(const ProjectContext& projectContext)
            : ValueProcessorBase(projectContext, INPUT_TYPE::getThisIdentifier(), OUTPUT_TYPE::getThisIdentifier()) {}

      protected:
        virtual void processValue(UserLogger& userLogger, const Instance<const ValueFeature, INPUT_TYPE>& input,
                                  Instance<ValueFeature, OUTPUT_TYPE>& output) const = 0;

      public:
        void process(UserLogger& userLogger) override {
            const SimpleValueFeature& inputFeature = this->getInputValueFeature();
            SimpleValueFeature& outputFeature = this->getOutputValueFeature();
            outputFeature.backUpValue();
            const Instance<const ValueFeature, INPUT_TYPE> input{inputFeature};
            Instance<ValueFeature, OUTPUT_TYPE> output{outputFeature};
            processValue(userLogger, input, output);
            outputFeature.reconcileChangesFromBackup();
        }
    };

} // namespace babelwires
