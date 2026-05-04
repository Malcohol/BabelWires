#pragma once

#include <BaseLib/Log/ostreamLogListener.hpp>
#include <BaseLib/Log/unifiedLog.hpp>

#include <sstream>

namespace testUtils {
    /// Replaces the currently registed debug logger during its lifetime,
    /// and restores the old one when it dies.
    class TestLog : public babelwires::UnifiedLog {
      public:
        TestLog();
        ~TestLog();

      private:
        DebugLogger* m_oldDebugLogger;
    };

    /// Stores the log output and provides an API for querying it.
    class TestLogWithListener : public TestLog {
      public:
        TestLogWithListener();

        std::string getLogContents() const;

        /// Return true if the substring got logged.
        bool hasSubstring(std::string_view substring) const;

        bool hasSubstringIgnoreCase(std::string_view substring) const;

        /// Comparison where "/", "\", and "\\" are not distinguished and case is ignored.
        /// It cannot abstract away the more significant differences (such as absolute vs relative), but
        /// is good enough for some unit tests. There is also some small risk of a false positive.
        bool hasSubstringWhenPathsNormalized(std::string_view substring) const;

        /// Clear the output stream.
        void clear();

      private:
        std::ostringstream m_output;
        babelwires::OStreamLogListener m_listener;
    };
} // namespace testUtils
