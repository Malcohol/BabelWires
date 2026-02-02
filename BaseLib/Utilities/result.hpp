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

} // namespace babelwires

#define DO_OR_ERROR(EXPRESSION_THAT_RETURNS_RESULT)                                                                    \
    {                                                                                                                  \
        const auto doOrErrorResult = EXPRESSION_THAT_RETURNS_RESULT;                                                   \
        if (!doOrErrorResult) {                                                                                        \
            return std::unexpected(doOrErrorResult.error());                                                           \
        }                                                                                                              \
    }

#define BW_UNIQUE_NAME(X, Y) BW_COMBINE_HELPER(X, Y)
#define BW_COMBINE_HELPER(X, Y) X##Y

// We can't use a scope in this case since it would enclose the target expression.
// Instead use a file-unique variable name. That requires the whole statement to be placed on one
// line of code.
// clang-format off
#define ASSIGN_OR_ERROR(TARGET_EXPRESSION, EXPRESSION_THAT_RETURNS_RESULTT)                                            \
    auto BW_UNIQUE_NAME(assignOrErrorResult, __LINE__) = EXPRESSION_THAT_RETURNS_RESULTT; if (!BW_UNIQUE_NAME(assignOrErrorResult, __LINE__)) { return std::unexpected(BW_UNIQUE_NAME(assignOrErrorResult, __LINE__).error()); } TARGET_EXPRESSION = std::move(*BW_UNIQUE_NAME(assignOrErrorResult, __LINE__));
// clang-format on

// Transition helper
#define THROW_ON_ERROR(RESULT, EXCEPTION_TYPE)                                                                         \
    do {                                                                                                               \
        if (!(RESULT)) {                                                                                               \
            throw EXCEPTION_TYPE() << (RESULT).error().toString();                                                     \
        }                                                                                                              \
    } while (0);
