/**
 * A convenient base class for processors which perform the same operation on several input features,
 * producing several output features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BaseLib/Result/result.hpp>

namespace babelwires {
    constexpr int s_maxParallelFeatures = 16;

    /// ParallelProcessors should override this for their input type. An array of the right shape will be automatically
    /// added at the end of the field set. It is typical (but not required) for parallel processors have common input
    /// settings separate from the per-entry inputs. A subclass can use the Instance DSL to provide convenient access
    /// to the fields of the type. It should not be necessary to reference the array in the instance, since the
    /// processor's ProcessEntry call provides direct access.
    class BABELWIRESLIB_API ParallelProcessorInputBase : public RecordType {
      public:
        DOWNCASTABLE(ParallelProcessorInputBase, RecordType);

        ParallelProcessorInputBase(TypeExp&& typeExpOfThis, const TypeSystem& typeSystem, std::vector<RecordType::FieldDefinition> commonInput, ShortId arrayId, TypeExp entryType);
    };

    /// ParallelProcessors should override this for their output type. The type will contain an array of the right
    /// shape.
    /// Note: There is currently no support for parallel processors to generate some common output in addition to the
    /// per-entry outputs. Support could be added if a use-case arises.
    /// TODO: We could automatically create these output types using a RecordTypeConstructor. However, such a type could
    /// not be directly addressed in a versioning system. Have a think about whether that matters.
    class BABELWIRESLIB_API ParallelProcessorOutputBase : public RecordType {
      public:
        DOWNCASTABLE(ParallelProcessorOutputBase, RecordType);

        ParallelProcessorOutputBase(TypeExp&& typeExpOfThis, const TypeSystem& typeSystem, ShortId arrayId, TypeExp entryType);
    };

    /// A base class for a common shape of processor which performs the same operation on several input features,
    /// producing several output features. Organizing suitable processors this way should reduce the number of
    /// Nodes in the project.
    class BABELWIRESLIB_API ParallelProcessor : public Processor {
      public:
        ParallelProcessor(const Context& context, const TypeExp& parallelInput,
                          const TypeExp& parallelOutput);

      protected:
        Result processValue(UserLogger& userLogger, const ValueTreeNode& input,
                          ValueTreeNode& output) const override final;

        virtual Result processEntry(UserLogger& userLogger, const ValueTreeNode& input,
                                    const ValueTreeNode& inputEntry, ValueTreeNode& outputEntry) const = 0;
    };

} // namespace babelwires
