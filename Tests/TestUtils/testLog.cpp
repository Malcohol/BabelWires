#include <Tests/TestUtils/testLog.hpp>

#include <algorithm>
#include <cassert>
#include <cctype>

namespace {
    std::string normalizeForPathSubstringSearch(std::string_view text) {
        std::string normalized;
        normalized.reserve(text.size());

        bool previousWasSlash = false;
        for (const unsigned char ch : text) {
            char out = static_cast<char>(std::tolower(ch));
            if (out == '\\') {
                out = '/';
            }

            if ((out == '/') && previousWasSlash) {
                continue;
            }

            normalized.push_back(out);
            previousWasSlash = (out == '/');
        }

        return normalized;
    }
} // namespace

testUtils::TestLog::TestLog()
    : m_oldDebugLogger(babelwires::DebugLogger::swapGlobalDebugLogger(this)) {}

testUtils::TestLog::~TestLog() {
    DebugLogger* currentLogger = babelwires::DebugLogger::swapGlobalDebugLogger(m_oldDebugLogger);
    assert((currentLogger == this) && "Mismatch in registration of debug logger.");
}

testUtils::TestLogWithListener::TestLogWithListener()
    : m_listener(m_output, *this) {}

std::string testUtils::TestLogWithListener::getLogContents() const {
    return m_output.str();
}

bool testUtils::TestLogWithListener::hasSubstring(std::string_view substring) const {
    return getLogContents().find(substring) != std::string::npos;
}

bool testUtils::TestLogWithListener::hasSubstringIgnoreCase(std::string_view substring) const {
    const auto equalCharIgnoreCase = [](char a, char b) { return std::toupper(a) == std::toupper(b); };
    const std::string contents = getLogContents();
    return std::search(contents.begin(), contents.end(), substring.begin(), substring.end(), equalCharIgnoreCase) !=
           contents.end();
}

bool testUtils::TestLogWithListener::hasSubstringWhenPathsNormalized(std::string_view substring) const {
    return normalizeForPathSubstringSearch(getLogContents()).find(normalizeForPathSubstringSearch(substring)) !=
           std::string::npos;
}

void testUtils::TestLogWithListener::clear() {
    m_output = std::ostringstream();
}
