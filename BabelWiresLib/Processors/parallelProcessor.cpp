/**
 * A convenient base class for processors which perform the same operation on several input features,
 * producing several output features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/Processors/parallelProcessor.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/ValueTree/Utilities/modelUtilities.hpp>

#include <BabelWiresLib/Instance/instanceUtils.hpp>
#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Array/arrayValue.hpp>
#include <BabelWiresLib/ValueTree/valueTreeRoot.hpp>
#include <BabelWiresLib/ValueTree/valueTreePathUtils.hpp>

#include <algorithm>
#include <array>
#include <execution>
#include <numeric>

namespace {
    babelwires::TypeRef getParallelArray(babelwires::TypeRef&& entryType) {
        return babelwires::ArrayTypeConstructor::makeTypeRef(std::move(entryType), 1, 16);
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
    const auto& inputType = parallelInput.resolveAs<ParallelProcessorInputBase>(projectContext.m_typeSystem);
    assert(inputType && "The ParallelProcessor input type should be a ParallelProcessorInputBase");
    const auto& outputType = parallelOutput.resolveAs<ParallelProcessorOutputBase>(projectContext.m_typeSystem);
    assert(outputType && "The ParallelProcessor output type should be a ParallelProcessorOutputBase");
    assert(inputType->getFields().size() >= 1);
    assert(outputType->getFields().size() == 1);
    // Note: The two IDs don't have to have the same distinguisher and can be registered separately.
    assert((inputType->getFields()[inputType->getFields().size() - 1].m_identifier ==
            outputType->getFields()[0].m_identifier) &&
           "The ParallelProcessor input and output IDs must agree");
#endif
}

void babelwires::ParallelProcessor::processValue(UserLogger& userLogger, const ValueTreeNode& input,
                                                 ValueTreeNode& output) const {
    bool shouldProcessAll = false;
    // Iterate through all features _except_ for the array, look for changes to the common input.
    for (unsigned int i = 0; i < input.getNumChildren() - 1; ++i) {
        if (input.getChild(i)->isChanged(ValueTreeNode::Changes::SomethingChanged)) {
            shouldProcessAll = true;
        }
    }

    const auto& arrayInput = input.getChild(input.getNumChildren() - 1)->is<ValueTreeNode>();
    auto& arrayOutput = output.getChild(output.getNumChildren() - 1)->is<ValueTreeNode>();

    if (arrayInput.isChanged(ValueTreeNode::Changes::StructureChanged)) {
        // TODO: This is very inefficient in cases where a single entry has been added or removed.
        // In the old feature system I was able to maintain a mapping between input and output entries
        // to avoid this inefficiency. Perhaps that can be done with values too.
        shouldProcessAll = true;
        InstanceUtils::setArraySize(arrayOutput, arrayInput.getNumChildren());
    }

    struct EntryData {
        EntryData(const TypeSystem& typeSystem, unsigned int index, const ValueTreeNode& inputEntry,
                  const ValueTreeNode& outputEntry)
            : m_index(index)
            , m_inputEntry(inputEntry)
            , m_outputEntry(std::make_unique<ValueTreeRoot>(typeSystem, outputEntry.getTypeRef())) {
            m_outputEntry->setValue(outputEntry.getValue());
        }

        const unsigned int m_index;
        const ValueTreeNode& m_inputEntry;
        std::unique_ptr<ValueTreeRoot> m_outputEntry;
        std::string m_failureString;
    };
    std::vector<EntryData> entriesToProcess;
    entriesToProcess.reserve(s_maxParallelFeatures);

    const TypeSystem& typeSystem = input.getTypeSystem();

    for (unsigned int i = 0; i < arrayInput.getNumChildren(); ++i) {
        const ValueTreeNode& inputEntry = arrayInput.getChild(i)->is<ValueTreeNode>();
        if (shouldProcessAll || arrayInput.getChild(i)->isChanged(ValueTreeNode::Changes::SomethingChanged)) {
            ValueTreeNode& outputEntry = arrayOutput.getChild(i)->is<ValueTreeNode>();
            entriesToProcess.emplace_back(EntryData{typeSystem, i, inputEntry, outputEntry});
        }
    }

    bool isFailed = false;
    std::for_each(
#ifndef __APPLE__
        std::execution::par,
#endif
        entriesToProcess.begin(), entriesToProcess.end(), [this, &input, &userLogger, &isFailed](EntryData& data) {
            try {
                processEntry(userLogger, input, data.m_inputEntry, *(data.m_outputEntry));
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
                compositeException << newline << "Failure processing entry " << getPathTo(&entry.m_inputEntry) << ": "
                                   << entry.m_failureString;
                newline = "\n";
            }
        }
        throw compositeException;
    }

    ArrayValue newOutput = arrayOutput.getValue()->is<ArrayValue>();
    for (EntryData& data : entriesToProcess) {
        newOutput.setValue(data.m_index, data.m_outputEntry->getValue());
    }
    arrayOutput.setValue(std::move(newOutput));
}
