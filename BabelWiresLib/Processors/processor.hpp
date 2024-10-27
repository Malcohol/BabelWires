/**
 * A Processor defines a processing operation from an input feature to an output feature.
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

    /// Defines a processing operation from an input feature to an output feature.
    /// This should not store any state.
    // TODO: Perhaps the design could be re-done in terms of const functions.
    class Processor {
      public:
        Processor(const ProjectContext& projectContext, const TypeRef& inputTypeRef, const TypeRef& outputTypeRef);
        virtual ~Processor();

        /// Set values in the output feature based on values in the input feature.
        /// Implementation of process are allowed to throw. If they do, the ProcessorElement will be marked as failed,
        /// the exception will be logged, and the output feature will be set to default.
        void process(UserLogger& userLogger);
        ValueTreeNode& getInput();
        ValueTreeNode& getOutput();
        const ValueTreeNode& getInput() const;
        const ValueTreeNode& getOutput() const;

      protected:
        /// Note: Implementations do not need to worry about backing-up or resolving changes in the output feature.
        virtual void processValue(UserLogger& userLogger, const ValueTreeNode& inputFeature,
                                  ValueTreeNode& outputFeature) const = 0;

      protected:
        std::unique_ptr<ValueTreeRoot> m_inputValueTreeRoot;
        std::unique_ptr<ValueTreeRoot> m_outputValueTreeRoot;
    };

} // namespace babelwires
