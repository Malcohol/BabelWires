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
#include <BabelWiresLib/Processors/commonProcessor.hpp>
#include <BabelWiresLib/Features/rootFeature.hpp>

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
        ParallelProcessor(const ProjectContext& projectContext) : CommonProcessor(projectContext) {}

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

} // namespace babelwires
