/**
 * A Processor defines an operation from an input ValueTree to an output ValueTree
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/babelWiresLibExport.hpp>
#include <BabelWiresLib/TypeSystem/typePtr.hpp>

#include <BaseLib/Result/result.hpp>

#include <memory>

namespace babelwires {
    struct UserLogger;
    class ValueTreeNode;
    struct ProjectContext;
    class TypeExp;
    class ValueTreeRoot;

    /// A Processor defines an operation from an input ValueTree to an output ValueTree.
    /// This should not store any state.
    class BABELWIRESLIB_API Processor {
      public:
        Processor(const ProjectContext& projectContext, TypePtr inputTypeExp, TypePtr outputTypeExp);
        virtual ~Processor();

        /// Set values in the output based on values in the input.
        /// If processing fails, a failure Result is returned and the output will usually be set to default (see
        /// onFailure).
        Result process(UserLogger& userLogger);
        ValueTreeRoot& getInput();
        ValueTreeRoot& getOutput();
        const ValueTreeRoot& getInput() const;
        const ValueTreeRoot& getOutput() const;

      protected:
        /// Note: Implementations do not need to worry about backing-up or resolving changes in the output.
        virtual Result processValue(UserLogger& userLogger, const ValueTreeNode& input,
                                    ValueTreeNode& output) const = 0;

        /// If processValue returns a failure Result, then this is called.
        /// The default implementation sets the output to a default value of its type.
        /// This can be overridden if you want to report failure but have a more subtle effect on the output.
        virtual void onFailure() const;

      protected:
        std::unique_ptr<ValueTreeRoot> m_inputValueTreeRoot;
        std::unique_ptr<ValueTreeRoot> m_outputValueTreeRoot;
    };

} // namespace babelwires
