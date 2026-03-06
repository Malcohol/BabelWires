/**
 * Type for representing the result of an operation that may fail.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

// See error.hpp and resultDSL.hpp for user-friendly ways to create and handle results.

#include <concepts>
#include <expected>
#include <string>
#include <variant>

namespace babelwires {
    /// Class that stores an error message.
    class ErrorStorage {
      public:
        explicit ErrorStorage(std::string message)
            : m_message(std::move(message)) {}

        const std::string& toString() const { return m_message; }

      private:
        std::string m_message;
    };

    // Primary template: just an alias-like wrapper around std::expected.
    // We use inheritance so we can partially specialize for T&.
    template <typename T> class ResultT : public std::expected<T, ErrorStorage> {
        using Base = std::expected<T, ErrorStorage>;

      public:
        using Base::Base;
        using Base::operator*;
        using Base::operator->;
        using Base::operator bool;
        using Base::error;
        using Base::has_value;
        using Base::transform;
        using Base::value;

        // Allow implicit construction from convertible types (matching std::expected behavior)
        template <typename U>
            requires std::convertible_to<U, T> && (!std::same_as<std::decay_t<U>, ErrorStorage>) &&
                     (!std::same_as<std::decay_t<U>, ResultT>)
        ResultT(U&& val)
            : Base(std::in_place, std::forward<U>(val)) {}

        // Allow construction from ErrorStorage
        ResultT(ErrorStorage err)
            : Base(std::unexpected<ErrorStorage>(std::move(err))) {}

        // Allow construction from std::unexpected<ErrorStorage>
        ResultT(std::unexpected<ErrorStorage> err)
            : Base(std::move(err)) {}

        // In-place construction
        template <typename... Args>
            requires std::constructible_from<T, Args...>
        explicit ResultT(std::in_place_t, Args&&... args)
            : Base(std::in_place, std::forward<Args>(args)...) {}

                // Allow construction from a std::expected with a convertible value type
        template <typename U>
            requires std::convertible_to<U, T> && (!std::same_as<U, T>)
        ResultT(std::expected<U, ErrorStorage>&& other)
            : Base(other.has_value() ? Base(std::in_place, static_cast<T>(std::move(*other)))
                                     : Base(std::unexpected<ErrorStorage>(std::move(other.error())))) {}

        // Allow construction from std::expected<T, ErrorStorage> directly
        ResultT(std::expected<T, ErrorStorage>&& other)
            : Base(std::move(other)) {}

        ResultT(const std::expected<T, ErrorStorage>& other)
            : Base(other) {}

    };

    // Partial specialization for references (std::expected doesn't support these)
    template <typename T> class ResultT<T&> {
      public:
        ResultT(T& ref)
            : m_storage(&ref) {}
        ResultT(ErrorStorage error)
            : m_storage(std::move(error)) {}
        ResultT(std::unexpected<ErrorStorage> error)
            : m_storage(std::move(error.error())) {}

        explicit operator bool() const { return m_storage.index() == 0; }
        bool has_value() const { return m_storage.index() == 0; }
        T& operator*() const { return *std::get<0>(m_storage); }
        T* operator->() const { return std::get<0>(m_storage); }
        const ErrorStorage& error() const { return std::get<1>(m_storage); }

      private:
        std::variant<T*, ErrorStorage> m_storage;
    };

    // Specialization for void: delegate to std::expected<void, ErrorStorage>
    template <> class ResultT<void> : public std::expected<void, ErrorStorage> {
        using Base = std::expected<void, ErrorStorage>;

      public:
        using Base::operator bool;
        using Base::error;
        using Base::has_value;

        ResultT()
            : Base() {}
        ResultT(ErrorStorage err)
            : Base(std::unexpected<ErrorStorage>(std::move(err))) {}
        ResultT(std::unexpected<ErrorStorage> err)
            : Base(std::move(err)) {}
    };

    using Result = ResultT<void>;
} // namespace babelwires

// Helper macro during the exception-to-result transition.
#define THROW_ON_ERROR(RESULT, EXCEPTION_TYPE)                                                                         \
    do {                                                                                                               \
        if (!(RESULT)) {                                                                                               \
            throw EXCEPTION_TYPE() << (RESULT).error().toString();                                                     \
        }                                                                                                              \
    } while (0);
