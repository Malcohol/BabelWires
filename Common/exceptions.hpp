/**
 * Common exception classes.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "Common/types.hpp"
#include <sstream>
#include <stdexcept>
#include <type_traits>

namespace babelwires {

    /// Common base class of all exceptions thrown by this project.
    /// You don't directly throw these.
    /// Based on http://marknelson.us/2007/11/13/no-exceptions/
    class BaseException : public std::exception {
      public:
        BaseException(const BaseException& other);
        virtual ~BaseException();

        virtual const char* what() const noexcept;

      protected:
        BaseException() {}
        std::ostream& getOStream() { return m_os; }

      private:
        mutable std::string m_what;
        mutable std::ostringstream m_os;
    };

    /// Mixin template which gives an exception class a << operator.
    template <typename EXCEPTION, typename PARENT = BaseException> class ExceptionWithStream : public PARENT {
      public:
        template <typename T> EXCEPTION& operator<<(T&& t) {
            PARENT::getOStream() << std::forward<T>(t);
            return static_cast<EXCEPTION&>(*this);
        }

      protected:
        ExceptionWithStream() = default;
    };

    class IoException : public ExceptionWithStream<IoException> {};
    class AudioIoException : public ExceptionWithStream<AudioIoException, IoException> {};
    class FileIoException : public ExceptionWithStream<FileIoException, IoException> {};
    class ParseException : public ExceptionWithStream<ParseException, FileIoException> {};

    class MathException : public ExceptionWithStream<MathException> {};

    /// For command-line parsing errors.
    class OptionError : public ExceptionWithStream<OptionError> {};

} // namespace babelwires
