/**
 * A set of macros for working with Results, designed to be used in functions that return a Result or ResultT, and allow
 * for easy propagation of errors without needing to write boilerplate code. See ResultTests.cpp for examples.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Result/result.hpp>
#include <BaseLib/Utilities/utilityMacros.hpp>

#include <cassert>

namespace babelwires {
    namespace Detail {
        /// When extracting from a ResultT<T>, we want to move the value out.
        /// When extracting from a ResultT<T&>, we want to return the reference as-is.
        template <typename T> decltype(auto) extractValue(ResultT<T>& result) {
            return std::move(*result);
        }

        template <typename T> T& extractValue(ResultT<T&>& result) {
            return *result;
        }

        template <typename T> T assertNoError(const ResultT<T>& result) {
            assert(result.has_value() && "No error expected");
            return *result;
        }
    } // namespace Detail
} // namespace babelwires

// Helper macros

// Dispatch macros: extract the last argument as the expression, reassemble the rest as the target.
#define BW_ASSIGN_2(a, expr) BW_ASSIGN_IMPL(expr, a)
#define BW_ASSIGN_3(a, b, expr) BW_ASSIGN_IMPL(expr, a, b)
#define BW_ASSIGN_4(a, b, c, expr) BW_ASSIGN_IMPL(expr, a, b, c)
#define BW_ASSIGN_5(a, b, c, d, expr) BW_ASSIGN_IMPL(expr, a, b, c, d)
#define BW_ASSIGN_6(a, b, c, d, e, expr) BW_ASSIGN_IMPL(expr, a, b, c, d, e)

// We can't use a scope in this case since it would enclose the target expression.
// Instead it uses a file-unique variable name.
#define BW_ASSIGN_IMPL(EXPRESSION_THAT_RETURNS_RESULTT, ...)                                                           \
    auto BW_CONCAT(assignOrErrorResult, __LINE__) = EXPRESSION_THAT_RETURNS_RESULTT;                              \
    if (!BW_CONCAT(assignOrErrorResult, __LINE__)) [[unlikely]] {                                                 \
        babelwiresOnError();                                                                                           \
        return std::unexpected(BW_CONCAT(assignOrErrorResult, __LINE__).error());                                 \
    }                                                                                                                  \
    __VA_ARGS__ = ::babelwires::Detail::extractValue(BW_CONCAT(assignOrErrorResult, __LINE__));
