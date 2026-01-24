/**
 * Common exception classes.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/exceptions.hpp>

#include <sstream>

babelwires::BaseException::~BaseException() {}

babelwires::BaseException::BaseException(const BaseException& other) {
    m_what += other.m_what;
    m_what += other.m_os.str();
}

const char* babelwires::BaseException::what() const noexcept {
    if (m_os.str().size()) {
        m_what += m_os.str();
        m_os.str("");
    }
    return m_what.c_str();
}
