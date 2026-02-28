/**
 * A set of macros for working with Results, designed to be used in functions that return a Result or ResultT, and allow
 * for easy propagation of errors without needing to write boilerplate code. See ResultTests.cpp for examples.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

// Assume that code that needs these macros needs wants Error too.
#include <BaseLib/Result/error.hpp>

// Helper macros.
#include <BaseLib/Result/resultDSLDetail.hpp>

// Helper macros for working with Results. These are designed to be used in functions that return a Result or ResultT,
// and allow for easy propagation of errors without needing to write boilerplate code. See ResultTests.cpp for examples.

/// Evaluate the expression, and if it returns an error, call babelwiresOnError and return the error.
#define DO_OR_ERROR(EXPRESSION_THAT_RETURNS_RESULT)                                                                    \
    {                                                                                                                  \
        const auto doOrErrorResult = EXPRESSION_THAT_RETURNS_RESULT;                                                   \
        if (!doOrErrorResult) [[unlikely]] {                                                                           \
            babelwiresOnError();                                                                                       \
            return std::unexpected(doOrErrorResult.error());                                                           \
        }                                                                                                              \
    }

/// Assign TARGET_EXPRESSION from the EXPRESSION_THAT_RETURNS_RESULTT, which must return a ResultT.
/// If the result is an error, call babelwiresOnError and return the error.
/// The target expression may contain commas (e.g. structured bindings like auto [a, b]), since the
/// macro is variadic: the last argument is the expression, and all preceding arguments form the target.
#define ASSIGN_OR_ERROR(...)                                                                                           \
    BW_CONCAT(BW_ASSIGN_, BW_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

/// Perform the code if an error occurs with the scope.
/// This cannot be used twice in the same scope, but you can introduce a new scope to work around that.
/// Note: Error handlers are automatically called by DO_OR_ERROR and ASSIGN_OR_ERROR, but they won't
/// be automatically called by code that explicitly returns an error. The RETURN_ERROR_VALUE and
/// RETURN_ERROR macros are provided as a standard way to ensure handlers are called in such situations.
#define ON_ERROR(CODE_TO_DO_ON_ERROR)                                                                                  \
    const auto babelwiresOnError2 = babelwiresOnError;                                                                 \
    const auto babelwiresOnError = [&]() {                                                                             \
        CODE_TO_DO_ON_ERROR;                                                                                           \
        babelwiresOnError2();                                                                                          \
    };

/// Return an error value, calling any ON_ERROR handlers in the scope before returning.
#define RETURN_ERROR_VALUE(ERROR)                                                                                      \
    babelwiresOnError();                                                                                               \
    return ERROR

/// Return a newly constructed error, calling any ON_ERROR handlers in the scope before returning.
#define RETURN_ERROR() RETURN_ERROR_VALUE(babelwires::Error())

/// The default error handler, which does nothing. It must be in the global namespace, because it is
/// intended to get shadowed by a local variable when ON_ERROR is used.
inline void babelwiresOnError() {}
