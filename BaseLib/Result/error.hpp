/**
 * Class to allow error messages to be easily built.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/Result/result.hpp>

#include <sstream>

namespace babelwires {
    /// Class for building an error message. It implicitly converts to an ErrorStorage and a ResultT, so it can be returned from a function that returns a ResultT.
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
