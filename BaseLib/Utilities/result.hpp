/**
 * Class for representing an error.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <expected>
#include <sstream>
#include <string>

namespace babelwires {
    class ErrorStorage;

    /// Result type which is either a T or an Error.
    template <typename T> using ResultT = std::expected<T, ErrorStorage>;

    /// Simple result type for success/failure operations (replaces old Result class).
    using Result = ResultT<void>;

    class ErrorStorage {
      public:
        explicit ErrorStorage(std::string message)
            : m_message(std::move(message)) {}

        const std::string& toString() const { return m_message; }

        /// Allow implicit conversion to a ResultT.
        template <typename T> operator ResultT<T>() { return std::unexpected<ErrorStorage>(std::move(*this)); }

      private:
        std::string m_message;
    };

    /// Class representing an error.
    class Error {
      public:
        Error() = default;
        Error(Error&&) = default;
        Error& operator=(Error&&) = default;

        // Copy operations not available since ostringstream is not copyable
        Error(const Error&) = delete;
        Error& operator=(const Error&) = delete;

        template <typename T> Error& operator<<(T&& t) {
            m_os << std::forward<T>(t);
            return *this;
        }

        Error& operator<<(const ErrorStorage& e) {
            m_os << e.toString();
            return *this;
        }
        Error& operator<<(ErrorStorage&& e) {
            m_os << e.toString();
            return *this;
        }
        Error& operator<<(ErrorStorage& e) {
            m_os << e.toString();
            return *this;
        }

        /// Allow implicit conversion to a ResultT.
        template <typename T> operator ResultT<T>() { return std::unexpected<ErrorStorage>(ErrorStorage(m_os.str())); }

        /// Allow implicit conversion to an ErrorStorage.
        operator ErrorStorage() { return ErrorStorage(m_os.str()); }

      private:
        std::ostringstream m_os;
    };

    /// Common functionality.
    enum class ErrorState { NoError, Error };

} // namespace babelwires

// Helper macros for working with Results. These are designed to be used in functions that return a Result or ResultT,
// and allow for easy propagation of errors without needing to write boilerplate code. See tests for examples.

/// By default, this on-error function is called.
/// It must be in the global namespace, because it is intended to get shadowed by a local variable when ON_ERROR is
/// used.
inline void babelwiresOnError() {}

/// Evaluate the expression, and if it returns an error, call babelwiresOnError and return the error.
#define DO_OR_ERROR(EXPRESSION_THAT_RETURNS_RESULT)                                                                    \
    {                                                                                                                  \
        const auto doOrErrorResult = EXPRESSION_THAT_RETURNS_RESULT;                                                   \
        if (!doOrErrorResult) {                                                                                        \
            babelwiresOnError();                                                                                       \
            return std::unexpected(doOrErrorResult.error());                                                           \
        }                                                                                                              \
    }

// Helper macros
#define BW_UNIQUE_NAME(X, Y) BW_COMBINE_HELPER(X, Y)
#define BW_COMBINE_HELPER(X, Y) X##Y

/// Assign TARGET_EXPRESSION from the EXPRESSION_THAT_RETURNS_RESULTT, which must return a ResultT.
/// If the result is an error, call babelwiresOnError and return the error.
// We can't use a scope in this case since it would enclose the target expression.
// Instead use a file-unique variable name.
#define ASSIGN_OR_ERROR(TARGET_EXPRESSION, EXPRESSION_THAT_RETURNS_RESULTT)                                            \
    auto BW_UNIQUE_NAME(assignOrErrorResult, __LINE__) = EXPRESSION_THAT_RETURNS_RESULTT;                              \
    if (!BW_UNIQUE_NAME(assignOrErrorResult, __LINE__)) {                                                              \
        babelwiresOnError();                                                                                           \
        return std::unexpected(BW_UNIQUE_NAME(assignOrErrorResult, __LINE__).error());                                 \
    }                                                                                                                  \
    TARGET_EXPRESSION = std::move(*BW_UNIQUE_NAME(assignOrErrorResult, __LINE__));

/// Perform the code if an error occurs with the scope.
/// This cannot be used twice in the same scope, but you can introduce a new scope to work around that.
#define ON_ERROR(CODE_TO_DO_ON_ERROR)                                                                                  \
    const auto babelwiresOnError2 = babelwiresOnError;                                                                 \
    const auto babelwiresOnError = [&]() {                                                                             \
        CODE_TO_DO_ON_ERROR;                                                                                           \
        babelwiresOnError2();                                                                                          \
    };

namespace babelwires {
    namespace Detail {
        /// Helper class for running code at the end of a scope.
        template <typename F> struct Finally {
            F m_func;
            explicit Finally(F f)
                : m_func(std::move(f)) {}
            ~Finally() { m_func(); }
            Finally(const Finally&) = delete;
            Finally& operator=(const Finally&) = delete;
        };
    } // namespace Detail
} // namespace babelwires

/// Run the code at the end of the scope.
#define FINALLY(CODE_TO_RUN)                                                                                           \
    const auto BW_UNIQUE_NAME(babelwiresFinally_, __LINE__) = ::babelwires::Detail::Finally([&]() { CODE_TO_RUN; });

/// Run the code at the end of the scope. The code can query a variable called errorState to determine whether the scope
/// is being exited due to an error or not.
#define FINALLY_WITH_ERRORSTATE(CODE_TO_RUN)                                                                           \
    babelwires::ErrorState BW_UNIQUE_NAME(babelwiresFinallyErrorState_, __LINE__) = babelwires::ErrorState::NoError;   \
    ON_ERROR(BW_UNIQUE_NAME(babelwiresFinallyErrorState_, __LINE__) = babelwires::ErrorState::Error);                  \
    const auto BW_UNIQUE_NAME(babelwiresFinally_, __LINE__) = ::babelwires::Detail::Finally(                           \
        [&, &errorState = BW_UNIQUE_NAME(babelwiresFinallyErrorState_, __LINE__)]() { CODE_TO_RUN; });

// Transition helper
#define THROW_ON_ERROR(RESULT, EXCEPTION_TYPE)                                                                         \
    do {                                                                                                               \
        if (!(RESULT)) {                                                                                               \
            throw EXCEPTION_TYPE() << (RESULT).error().toString();                                                     \
        }                                                                                                              \
    } while (0);
