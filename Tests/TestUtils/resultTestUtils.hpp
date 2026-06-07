#pragma once

#include <BaseLib/Result/resultDSLDetail.hpp>

// Detail macros

#define BW_ASSERT_RESULT_ASSIGN_IMPL(EXPRESSION_THAT_RETURNS_RESULTT, ...)                                                         \
    auto BW_CONCAT(assignOrErrorResult, __LINE__) = EXPRESSION_THAT_RETURNS_RESULTT;                              \
    ASSERT_TRUE(BW_CONCAT(assignOrErrorResult, __LINE__).has_value());                                            \
    __VA_ARGS__ = ::babelwires::Detail::extractValue(BW_CONCAT(assignOrErrorResult, __LINE__));

#define BW_ASSERT_RESULT_ASSIGN_2(a, expr) BW_ASSERT_RESULT_ASSIGN_IMPL(expr, a)
#define BW_ASSERT_RESULT_ASSIGN_3(a, b, expr) BW_ASSERT_RESULT_ASSIGN_IMPL(expr, a, b)
#define BW_ASSERT_RESULT_ASSIGN_4(a, b, c, expr) BW_ASSERT_RESULT_ASSIGN_IMPL(expr, a, b, c)
#define BW_ASSERT_RESULT_ASSIGN_5(a, b, c, d, expr) BW_ASSERT_RESULT_ASSIGN_IMPL(expr, a, b, c, d)
#define BW_ASSERT_RESULT_ASSIGN_6(a, b, c, d, e, expr) BW_ASSERT_RESULT_ASSIGN_IMPL(expr, a, b, c, d, e)

/// Use this to ASSERT_TRUE that the result of a result assignment was not an error.
#define BW_ASSERT_RESULT_ASSIGN(...) BW_CONCAT(BW_ASSERT_RESULT_ASSIGN_, BW_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)
