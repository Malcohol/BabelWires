/**
 * A convenient base class for processors which perform the same operation on several input features,
 * producing several output features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/parallelProcessor.hpp>

#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>
#include <BabelWiresLib/Features/Path/featurePath.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Array/arrayValue.hpp>
#include <BabelWiresLib/Instance/instanceUtils.hpp>

#include <algorithm>
#include <numeric>
#include <array>
#include <execution>

void babelwires::Detail::parallelProcessorHelper(
    const RecordFeature* inputFeature, const babelwires::ArrayFeature* arrayIn, babelwires::ArrayFeature* arrayOut,
    babelwires::UserLogger& userLogger,
    std::function<void(babelwires::UserLogger&, const Feature*, Feature*)> processEntry) {
    if (arrayIn->isChanged(babelwires::Feature::Changes::StructureChanged)) {
        // Keep the out structure in sync with the in structure, and
        // try to ensure that tracks which correspond before still
        // correspond afterwards.
        arrayOut->copyStructureFrom(*arrayIn);
    }

    bool nonArrayInputChange = false;
    for (auto i : subfeatures(inputFeature)) {
        if ((i != arrayIn) && i->isChanged(Feature::Changes::SomethingChanged)) {
            nonArrayInputChange = true;
            break;
        }
    }

    std::array<int, s_maxParallelFeatures> indices;
    bool isFailed = false;
    std::array<std::string, s_maxParallelFeatures> failureStrings;
    std::iota(indices.begin(), indices.end(), 0);
    std::for_each(
#ifndef __APPLE__
        std::execution::par,
#endif
        indices.begin(), indices.begin() + arrayIn->getNumFeatures(),
                  [&arrayIn, &arrayOut, &userLogger, &failureStrings, &isFailed, &processEntry, nonArrayInputChange](int i) {
                      const Feature* inputEntry = arrayIn->getFeature(i);
                      if (nonArrayInputChange || inputEntry->isChanged(Feature::Changes::SomethingChanged)) {
                        try {
                            processEntry(userLogger, inputEntry, arrayOut->getFeature(i));
                        } catch (const BaseException& e) {
                            failureStrings[i] = e.what();
                            isFailed = true;
                        }
                      }
                  });
    if (isFailed) {
        // TODO: Need a more precise way to signal failure.
        ModelException compositeException;
        const char* newline = "";
        for (int i = 0; i < failureStrings.size(); ++i) {
            if (!failureStrings[i].empty()) {
                compositeException << newline << "Failure processing entry " << FeaturePath(arrayIn->getFeature(i)) << ": " << failureStrings[i];
                newline = "\n";
            }
        }
        throw compositeException;
    }
}

namespace {
    babelwires::TypeRef getParallelArray(babelwires::TypeRef&& entryType) {
        return babelwires::TypeRef{
            babelwires::ArrayTypeConstructor::getThisIdentifier(),
            {{std::move(entryType)}, {babelwires::IntValue(1), babelwires::IntValue(16), babelwires::IntValue(1)}}};
    }

    std::vector<babelwires::RecordType::Field>&& addArray(std::vector<babelwires::RecordType::Field>&& commonData,
                                                          babelwires::ShortId arrayId, babelwires::TypeRef entryType) {
        commonData.emplace_back(babelwires::RecordType::Field{arrayId, getParallelArray(std::move(entryType))});
        return std::move(commonData);
    }
} // namespace

babelwires::ParallelValueProcessorInputBase::ParallelValueProcessorInputBase(std::vector<RecordType::Field> commonData,
                                                                             ShortId arrayId, TypeRef entryType)
    : RecordType(addArray(std::move(commonData), arrayId, entryType)) {}

babelwires::ParallelValueProcessorOutputBase::ParallelValueProcessorOutputBase(ShortId arrayId, TypeRef entryType)
    : RecordType({{arrayId, getParallelArray(std::move(entryType))}}) {}

babelwires::ParallelValueProcessor::ParallelValueProcessor(const ProjectContext& projectContext,
                                                           const TypeRef& parallelInput, const TypeRef& parallelOutput)
    : ValueProcessor(projectContext, parallelInput, parallelOutput) {
#ifndef NDEBUG
    const auto* inputType = parallelInput.resolve(projectContext.m_typeSystem).as<ParallelValueProcessorInputBase>();
    assert(inputType && "The ParallelValueProcessor input type should be a ParallelValueProcessorInputBase");
    const auto* outputType = parallelOutput.resolve(projectContext.m_typeSystem).as<ParallelValueProcessorOutputBase>();
    assert(outputType && "The ParallelValueProcessor output type should be a ParallelValueProcessorOutputBase");
    assert(inputType->getFields().size() >= 1);
    assert(outputType->getFields().size() == 1);
    // Note: The two IDs don't have to have the same distinguisher and can be registered separately.
    assert((inputType->getFields()[inputType->getFields().size() - 1].m_identifier ==
            outputType->getFields()[0].m_identifier) &&
           "The ParallelValueProcessor input and output IDs must agree");
#endif
}

void babelwires::ParallelValueProcessor::processValue(UserLogger& userLogger, const ValueFeature& inputFeature,
                                                      ValueFeature& outputFeature) const {
    bool shouldProcessAll = false;
    // Iterate through all features _except_ for the array, look for changes to the common input.
    for (unsigned int i = 0; i < inputFeature.getNumFeatures() - 1; ++i) {
        if (inputFeature.getFeature(i)->isChanged(Feature::Changes::SomethingChanged)) {
            shouldProcessAll = true;
        }
    }

    const auto& arrayInputFeature = inputFeature.getFeature(inputFeature.getNumFeatures() - 1)->is<ValueFeature>();
    auto& arrayOutputFeature = outputFeature.getFeature(outputFeature.getNumFeatures() - 1)->is<ValueFeature>();

    if (arrayInputFeature.isChanged(Feature::Changes::StructureChanged)) {
        // TODO: This is very inefficient in cases where a single entry has been added or removed.
        // In the old feature system I was able to maintain a mapping between input and output entries
        // to avoid this inefficiency. Perhaps that can be done with values too.
        shouldProcessAll = true;
        InstanceUtils::setArraySize(arrayOutputFeature, arrayInputFeature.getNumFeatures());
    }

    struct EntryData {
        EntryData(const TypeSystem& typeSystem, unsigned int index, const ValueFeature& inputEntry, const ValueFeature& outputEntry)
            : m_index(index)
            , m_inputEntry(inputEntry)
            , m_outputEntry(std::make_unique<SimpleValueFeature>(typeSystem, outputEntry.getTypeRef()))
            {
                m_outputEntry->setValue(outputEntry.getValue());
            }

        const unsigned int m_index;
        const ValueFeature& m_inputEntry;
        std::unique_ptr<SimpleValueFeature> m_outputEntry;
        std::string m_failureString;
    };
    std::vector<EntryData> entriesToProcess;
    entriesToProcess.reserve(s_maxParallelFeatures);

    const TypeSystem& typeSystem = inputFeature.getTypeSystem();

    for (unsigned int i = 0; i < arrayInputFeature.getNumFeatures(); ++i) {
        const ValueFeature& inputEntry = arrayInputFeature.getFeature(i)->is<ValueFeature>();
        if (shouldProcessAll || arrayInputFeature.getFeature(i)->isChanged(Feature::Changes::SomethingChanged)) {
            ValueFeature& outputEntry = arrayOutputFeature.getFeature(i)->is<ValueFeature>();
            entriesToProcess.emplace_back(EntryData{typeSystem, i, inputEntry, outputEntry});
        }
    }

    bool isFailed = false;
    std::for_each(
#ifndef __APPLE__
        std::execution::par,
#endif
        entriesToProcess.begin(), entriesToProcess.end(),
        [this, &inputFeature, &userLogger, &isFailed](EntryData& data) {
            try {
                processEntry(userLogger, inputFeature, data.m_inputEntry, *(data.m_outputEntry));
            } catch (const BaseException& e) {
                data.m_failureString = e.what();
                isFailed = true;
            }
        });
    
    if (isFailed) {
        // TODO: Need a more precise way to signal failure.
        ModelException compositeException;
        const char* newline = "";
        for (const auto& entry : entriesToProcess) {
            if (!entry.m_failureString.empty()) {
                compositeException << newline << "Failure processing entry " << FeaturePath(&entry.m_inputEntry) << ": "
                                   << entry.m_failureString;
                newline = "\n";
            }
        }
        throw compositeException;
    }

    ArrayValue newOutput = arrayOutputFeature.getValue()->is<ArrayValue>();
    for (EntryData& data : entriesToProcess) {
        newOutput.setValue(data.m_index, data.m_outputEntry->getValue());
    }
    arrayOutputFeature.setValue(newOutput);
}
