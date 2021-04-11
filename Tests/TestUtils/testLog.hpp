#pragma once

#include "Common/Log/ostreamLogListener.hpp"
#include "Common/Log/unifiedLog.hpp"

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

        /// Clear the output stream.
        void clear();

      private:
        std::ostringstream m_output;
        babelwires::OStreamLogListener m_listener;
    };
} // namespace testUtils
