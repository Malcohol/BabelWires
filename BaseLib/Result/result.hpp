/**
 * Type for representing the result of an operation that may fail.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <expected>
#include <string>

namespace babelwires {
    class ErrorStorage;

    /// Type for representing the result of an operation that may fail.
    /// See error.hpp and resultDSL.hpp for tools to aid working with this type.
    template <typename T> using ResultT = std::expected<T, ErrorStorage>;

    /// Simple result type for success/failure operations.
    using Result = ResultT<void>;

    /// Class that stores an error message. This is the type used in the error part of a ResultT.
    /// See Error for a class that helps with constructing an ErrorStorage.
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
}

// Helper macro during the exception-to-result transition.
#define THROW_ON_ERROR(RESULT, EXCEPTION_TYPE)                                                                         \
    do {                                                                                                               \
        if (!(RESULT)) {                                                                                               \
            throw EXCEPTION_TYPE() << (RESULT).error().toString();                                                     \
        }                                                                                                              \
    } while (0);
