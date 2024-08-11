/**
 * A convenient base class for processors which perform the same operation on several input features,
 * producing several output features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/Features/arrayFeature.hpp>
#include <BabelWiresLib/Features/featureMixins.hpp>
#include <BabelWiresLib/Features/modelExceptions.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>
#include <BabelWiresLib/Processors/commonProcessor.hpp>
#include <BabelWiresLib/Processors/valueProcessor.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

namespace babelwires {
    constexpr int s_maxParallelFeatures = 16;

    namespace Detail {
        /// This helper function carries code which doesn't need to be templated.
        void parallelProcessorHelper(const RecordFeature* inputFeature, const ArrayFeature* arrayIn,
                                     ArrayFeature* arrayOut, UserLogger& userLogger,
                                     std::function<void(UserLogger&, const Feature*, Feature*)> processEntry);
    } // namespace Detail

    /// A base class for a common shape of processor which performs the same operation on several input features,
    /// producing several output features. Organizing suitable processors this way should reduce the number of
    /// elements in the project.
    ///
    /// Subclasses should call addArrayFeature in their constructor, and need to override the pure virtual processEntry
    /// method.
    template <typename INPUT_ENTRY_FEATURE, typename OUTPUT_ENTRY_FEATURE>
    class ParallelProcessor : public CommonProcessor {
      public:
        ParallelProcessor(const ProjectContext& projectContext)
            : CommonProcessor(projectContext) {}

        using InputEntryFeature = INPUT_ENTRY_FEATURE;
        using OutputEntryFeature = OUTPUT_ENTRY_FEATURE;
        using InputArrayFeature = HasStaticSizeRange<StandardArrayFeature<InputEntryFeature>, 1, s_maxParallelFeatures>;
        using OutputArrayFeature =
            HasStaticSizeRange<StandardArrayFeature<OutputEntryFeature>, 1, s_maxParallelFeatures>;

        void process(UserLogger& userLogger) override {
            Detail::parallelProcessorHelper(m_inputFeature.get(), m_arrayIn, m_arrayOut, userLogger,
                                            [this](UserLogger& userLogger, const Feature* input, Feature* output) {
                                                const auto& featureIn = input->is<InputEntryFeature>();
                                                auto& featureOut = output->is<OutputEntryFeature>();
                                                processEntry(userLogger, featureIn, featureOut);
                                            });
        }

      protected:
        /// Subclass constructors should call this to add the input/output array. This should almost always
        /// be after all the input features are added.
        void addArrayFeature(ShortId id) {
            m_arrayIn = m_inputFeature->addField(std::make_unique<InputArrayFeature>(), id);
            m_arrayOut = m_outputFeature->addField(std::make_unique<OutputArrayFeature>(), id);
        }

        /// Subclasses must override this with the per-entry processing logic. This is only called if
        /// input has changed or some other input feature which isn't the array has changed.
        /// This method can signal errors by throwing an exception.
        virtual void processEntry(UserLogger& userLogger, const InputEntryFeature& input,
                                  OutputEntryFeature& output) const = 0;

      private:
        ArrayFeature* m_arrayIn = nullptr;
        ArrayFeature* m_arrayOut = nullptr;
    };

    /// ParallelProcessors should override this for their input type. An array of the right shape will be automatically
    /// added at the end of the field set. It is typical (but not required) for parallel processors have common input
    /// settings separate from the per-entry inputs. A subclass can use the Instance DSL to provide convenient access
    /// to the fields of the type. It should not be necessary to reference the array in the instance, since the
    /// processor's ProcessEntry call provides direct access.
    class ParallelValueProcessorInputBase : public RecordType {
      public:
        ParallelValueProcessorInputBase(std::vector<RecordType::Field> commonInput, ShortId arrayId, TypeRef entryType);
    };

    /// ParallelProcessors should override this for their output type. The type will contain an array of the right
    /// shape.
    /// Note: There is currently no support for parallel processors to generate some common output in addition to the
    /// per-entry outputs. Support could be added if a use-case arises.
    /// TODO: We could automatically create these output types using a RecordTypeConstructor. However, such a type could
    /// not be directly addressed in a versioning system. Have a think about whether that matters.
    class ParallelValueProcessorOutputBase : public RecordType {
      public:
        ParallelValueProcessorOutputBase(ShortId arrayId, TypeRef entryType);
    };

    /// A base class for a common shape of processor which performs the same operation on several input features,
    /// producing several output features. Organizing suitable processors this way should reduce the number of
    /// elements in the project.
    class ParallelValueProcessor : public ValueProcessor {
      public:
        ParallelValueProcessor(const ProjectContext& projectContext, const TypeRef& parallelInput,
                               const TypeRef& parallelOutput);

      protected:
        void processValue(UserLogger& userLogger, const ValueFeature& inputFeature,
                          ValueFeature& outputFeature) const override final;

        virtual void processEntry(UserLogger& userLogger, const ValueFeature& inputFeature,
                                  const ValueFeature& inputEntry, ValueFeature& outputEntry) const = 0;
    };

} // namespace babelwires
