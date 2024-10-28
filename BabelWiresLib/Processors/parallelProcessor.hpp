/**
 * A convenient base class for processors which perform the same operation on several input features,
 * producing several output features.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/ValueTree/modelExceptions.hpp>
#include <BabelWiresLib/Processors/processor.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

namespace babelwires {
    constexpr int s_maxParallelFeatures = 16;

    /// ParallelProcessors should override this for their input type. An array of the right shape will be automatically
    /// added at the end of the field set. It is typical (but not required) for parallel processors have common input
    /// settings separate from the per-entry inputs. A subclass can use the Instance DSL to provide convenient access
    /// to the fields of the type. It should not be necessary to reference the array in the instance, since the
    /// processor's ProcessEntry call provides direct access.
    class ParallelProcessorInputBase : public RecordType {
      public:
        ParallelProcessorInputBase(std::vector<RecordType::Field> commonInput, ShortId arrayId, TypeRef entryType);
    };

    /// ParallelProcessors should override this for their output type. The type will contain an array of the right
    /// shape.
    /// Note: There is currently no support for parallel processors to generate some common output in addition to the
    /// per-entry outputs. Support could be added if a use-case arises.
    /// TODO: We could automatically create these output types using a RecordTypeConstructor. However, such a type could
    /// not be directly addressed in a versioning system. Have a think about whether that matters.
    class ParallelProcessorOutputBase : public RecordType {
      public:
        ParallelProcessorOutputBase(ShortId arrayId, TypeRef entryType);
    };

    /// A base class for a common shape of processor which performs the same operation on several input features,
    /// producing several output features. Organizing suitable processors this way should reduce the number of
    /// elements in the project.
    class ParallelProcessor : public Processor {
      public:
        ParallelProcessor(const ProjectContext& projectContext, const TypeRef& parallelInput,
                          const TypeRef& parallelOutput);

      protected:
        void processValue(UserLogger& userLogger, const ValueTreeNode& input,
                          ValueTreeNode& output) const override final;

        virtual void processEntry(UserLogger& userLogger, const ValueTreeNode& input,
                                  const ValueTreeNode& inputEntry, ValueTreeNode& outputEntry) const = 0;
    };

} // namespace babelwires
