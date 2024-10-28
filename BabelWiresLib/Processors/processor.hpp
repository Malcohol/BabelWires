/**
 * A Processor defines an operation from an input ValueTree to an output ValueTree
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <memory>

namespace babelwires {
    struct UserLogger;
    class ValueTreeNode;
    struct ProjectContext;
    class TypeRef;
    class ValueTreeRoot;

    /// A Processor defines an operation from an input ValueTree to an output ValueTree.
    /// This should not store any state.
    class Processor {
      public:
        Processor(const ProjectContext& projectContext, const TypeRef& inputTypeRef, const TypeRef& outputTypeRef);
        virtual ~Processor();

        /// Set values in the output based on values in the input.
        /// Implementation of process are allowed to throw. If they do, the ProcessorElement will be marked as failed,
        /// the exception will be logged and the output will be set to default.
        void process(UserLogger& userLogger);
        ValueTreeNode& getInput();
        ValueTreeNode& getOutput();
        const ValueTreeNode& getInput() const;
        const ValueTreeNode& getOutput() const;

      protected:
        /// Note: Implementations do not need to worry about backing-up or resolving changes in the output.
        virtual void processValue(UserLogger& userLogger, const ValueTreeNode& input,
                                  ValueTreeNode& output) const = 0;

      protected:
        std::unique_ptr<ValueTreeRoot> m_inputValueTreeRoot;
        std::unique_ptr<ValueTreeRoot> m_outputValueTreeRoot;
    };

} // namespace babelwires
