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
        /// Implementations of process are allowed to throw. If they do, the ProcessorNode will be marked as failed,
        /// the exception will be logged and the output will usually be set to default (see onFailure).
        void process(UserLogger& userLogger);
        ValueTreeRoot& getInput();
        ValueTreeRoot& getOutput();
        const ValueTreeRoot& getInput() const;
        const ValueTreeRoot& getOutput() const;

      protected:
        /// Note: Implementations do not need to worry about backing-up or resolving changes in the output.
        virtual void processValue(UserLogger& userLogger, const ValueTreeNode& input, ValueTreeNode& output) const = 0;

        /// If an exception is thrown by processValue, then this is called.
        /// The default implementation sets the output to a default value of its type.
        /// This can be overridden if you want to report failure but have a more subtle effect on the output.
        virtual void onFailure() const;

      protected:
        std::unique_ptr<ValueTreeRoot> m_inputValueTreeRoot;
        std::unique_ptr<ValueTreeRoot> m_outputValueTreeRoot;
    };

} // namespace babelwires
