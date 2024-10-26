/**
 * A convenient base class for processors which perform the same operation on several input features,
 * producing several output features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/parallelProcessor.hpp>

#include <BabelWiresLib/Features/Path/featurePath.hpp>
#include <BabelWiresLib/Features/Utilities/modelUtilities.hpp>

#include <BabelWiresLib/Features/simpleValueFeature.hpp>
#include <BabelWiresLib/Instance/instanceUtils.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Array/arrayValue.hpp>

#include <algorithm>
#include <array>
#include <execution>
#include <numeric>

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

babelwires::ParallelProcessorInputBase::ParallelProcessorInputBase(std::vector<RecordType::Field> commonData,
                                                                   ShortId arrayId, TypeRef entryType)
    : RecordType(addArray(std::move(commonData), arrayId, entryType)) {}

babelwires::ParallelProcessorOutputBase::ParallelProcessorOutputBase(ShortId arrayId, TypeRef entryType)
    : RecordType({{arrayId, getParallelArray(std::move(entryType))}}) {}

babelwires::ParallelProcessor::ParallelProcessor(const ProjectContext& projectContext, const TypeRef& parallelInput,
                                                 const TypeRef& parallelOutput)
    : Processor(projectContext, parallelInput, parallelOutput) {
#ifndef NDEBUG
    const auto* inputType = parallelInput.resolve(projectContext.m_typeSystem).as<ParallelProcessorInputBase>();
    assert(inputType && "The ParallelProcessor input type should be a ParallelProcessorInputBase");
    const auto* outputType = parallelOutput.resolve(projectContext.m_typeSystem).as<ParallelProcessorOutputBase>();
    assert(outputType && "The ParallelProcessor output type should be a ParallelProcessorOutputBase");
    assert(inputType->getFields().size() >= 1);
    assert(outputType->getFields().size() == 1);
    // Note: The two IDs don't have to have the same distinguisher and can be registered separately.
    assert((inputType->getFields()[inputType->getFields().size() - 1].m_identifier ==
            outputType->getFields()[0].m_identifier) &&
           "The ParallelProcessor input and output IDs must agree");
#endif
}

void babelwires::ParallelProcessor::processValue(UserLogger& userLogger, const Feature& inputFeature,
                                                 Feature& outputFeature) const {
    bool shouldProcessAll = false;
    // Iterate through all features _except_ for the array, look for changes to the common input.
    for (unsigned int i = 0; i < inputFeature.getNumFeatures() - 1; ++i) {
        if (inputFeature.getFeature(i)->isChanged(Feature::Changes::SomethingChanged)) {
            shouldProcessAll = true;
        }
    }

    const auto& arrayInputFeature = inputFeature.getFeature(inputFeature.getNumFeatures() - 1)->is<Feature>();
    auto& arrayOutputFeature = outputFeature.getFeature(outputFeature.getNumFeatures() - 1)->is<Feature>();

    if (arrayInputFeature.isChanged(Feature::Changes::StructureChanged)) {
        // TODO: This is very inefficient in cases where a single entry has been added or removed.
        // In the old feature system I was able to maintain a mapping between input and output entries
        // to avoid this inefficiency. Perhaps that can be done with values too.
        shouldProcessAll = true;
        InstanceUtils::setArraySize(arrayOutputFeature, arrayInputFeature.getNumFeatures());
    }

    struct EntryData {
        EntryData(const TypeSystem& typeSystem, unsigned int index, const Feature& inputEntry,
                  const Feature& outputEntry)
            : m_index(index)
            , m_inputEntry(inputEntry)
            , m_outputEntry(std::make_unique<SimpleValueFeature>(typeSystem, outputEntry.getTypeRef())) {
            m_outputEntry->setValue(outputEntry.getValue());
        }

        const unsigned int m_index;
        const Feature& m_inputEntry;
        std::unique_ptr<SimpleValueFeature> m_outputEntry;
        std::string m_failureString;
    };
    std::vector<EntryData> entriesToProcess;
    entriesToProcess.reserve(s_maxParallelFeatures);

    const TypeSystem& typeSystem = inputFeature.getTypeSystem();

    for (unsigned int i = 0; i < arrayInputFeature.getNumFeatures(); ++i) {
        const Feature& inputEntry = arrayInputFeature.getFeature(i)->is<Feature>();
        if (shouldProcessAll || arrayInputFeature.getFeature(i)->isChanged(Feature::Changes::SomethingChanged)) {
            Feature& outputEntry = arrayOutputFeature.getFeature(i)->is<Feature>();
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
                compositeException << newline << "Failure processing entry " << Path(&entry.m_inputEntry) << ": "
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
