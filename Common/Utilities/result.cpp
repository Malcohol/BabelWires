/**
 * A result class which can carry an explanation why it failed.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <Common/Utilities/result.hpp>

#include <cassert>

babelwires::Result::Result(std::string reasonWhyFailed)
    : m_reasonWhyFailed(reasonWhyFailed) {
    assert(!reasonWhyFailed.empty() && "A reason for failure must be provided");
}

babelwires::Result::Result(const char* reasonWhyFailed)
    : Result(std::string(reasonWhyFailed)) {
}

std::string babelwires::Result::getReasonWhyFailed() const {
    assert(!m_reasonWhyFailed.empty() && "There was no failure.");
    return m_reasonWhyFailed;
}
