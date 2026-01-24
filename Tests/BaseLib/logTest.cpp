#include <BaseLib/Log/ostreamLogListener.hpp>
#include <BaseLib/Log/unifiedLog.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <thread>

using namespace babelwires;

TEST(LogTest, withDebug) {
    std::ostringstream os;
    UnifiedLog log;
    OStreamLogListener logToCout(os, log, OStreamLogListener::Features::logDebugMessages);

    log.logError() << "Foo " << 0 << " bar";
    log.logDebug() << "Foo " << 1 << " bar";
    log.logWarning() << "Foo " << 2 << " bar";
    log.logInfo() << "Foo " << 3 << " bar";

    EXPECT_EQ(os.str(), "Error: Foo 0 bar\nDebug: Foo 1 bar\nWarning: Foo 2 bar\nInfo: Foo 3 bar\n");
}

TEST(LogTest, withTimestamp) {
    std::ostringstream os;
    UnifiedLog log;
    OStreamLogListener logToCout(os, log, OStreamLogListener::Features() | OStreamLogListener::Features::timestamp);

    Log::TimeStamp now = Log::TimeStampClock::now();
    log.logWarning() << "Foo 6";

    std::string logContents = os.str(); //"2020-04-12 15:33:24.759 +0100 Warning: Foo 6\n";// os.str();

    std::tm tm = {0};
    tm.tm_isdst = -1;

    int month = 0;
    int millisecs = 0;
    char offsetBuf[256] = {0};
    int six = 0;

    // "2020-04-12 15:33:24.759 +0100 Warning: Foo 6\n"
    EXPECT_EQ(sscanf(logContents.c_str(), "%d-%d-%d %d:%d:%d.%d %s Warning: Foo %d\n", &tm.tm_year, &tm.tm_mon,
                     &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec, &millisecs, offsetBuf, &six),
              9);
    EXPECT_GE(millisecs, 0);
    EXPECT_LT(millisecs, 1000);
    EXPECT_TRUE(offsetBuf[0] == '-' || offsetBuf[0] == '+');
    EXPECT_TRUE(offsetBuf[5] == '\0');
    EXPECT_EQ(six, 6);

    tm.tm_year -= 1900;
    tm.tm_mon -= 1;

    const std::time_t testTime = std::mktime(&tm);
    const std::time_t logTime = Log::TimeStampClock::to_time_t(now);

    EXPECT_NEAR(std::difftime(logTime, testTime), 0.0, 1.0);
}

TEST(LogTest, withoutDebug) {
    std::ostringstream os;
    UnifiedLog log;
    OStreamLogListener logToCout(os, log, OStreamLogListener::Features());

    log.logError() << "Foo " << 0 << " bar";
    log.logDebug() << "Foo " << 1 << " bar";
    log.logWarning() << "Foo " << 2 << " bar";
    log.logInfo() << "Foo " << 3 << " bar";

    EXPECT_EQ(os.str(), "Error: Foo 0 bar\nWarning: Foo 2 bar\nInfo: Foo 3 bar\n");
}

TEST(LogTest, threadSafe) {
    std::ostringstream os;
    UnifiedLog log;
    OStreamLogListener logToCout(os, log, OStreamLogListener::Features());

    const auto logAMessage = [&log]() {
        using namespace std::chrono_literals;

        auto error = log.logError();
        std::this_thread::sleep_for(10ms);
        error << "Foo ";
        std::this_thread::sleep_for(10ms);
        error << 0 << " ";
        std::this_thread::sleep_for(10ms);
        error << 1 << " ";
        std::this_thread::sleep_for(10ms);
        error << 2 << " ";
        std::this_thread::sleep_for(10ms);
        error << 3 << " ";
        std::this_thread::sleep_for(10ms);
        error << 4 << " bar";
        std::this_thread::sleep_for(10ms);
    };

    const int count = 50;
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < count; ++i) {
            threads.emplace_back(logAMessage);
        }
        std::for_each(threads.begin(), threads.end(), [](auto& t) { t.join(); });
    }

    std::ostringstream expected;
    for (int i = 0; i < count; ++i) {
        expected << "Error: Foo 0 1 2 3 4 bar\n";
    }

    EXPECT_EQ(os.str(), expected.str());
}

TEST(LogTest, globalDebugLog) {
    std::ostringstream os;
    UnifiedLog log1;
    UnifiedLog log2;
    OStreamLogListener logToOs1(os, log1, OStreamLogListener::Features::logDebugMessages);
    OStreamLogListener logToOs2(os, log2, OStreamLogListener::Features::logDebugMessages);

    log1.logError() << "Foo 0";

    DebugLogger* old1 = DebugLogger::swapGlobalDebugLogger(&log1);
    EXPECT_EQ(old1, nullptr);
    logDebug() << "Foo 1";

    DebugLogger* old2 = DebugLogger::swapGlobalDebugLogger(&log2);
    EXPECT_EQ(old2, &log1);
    logDebug() << "Foo 2";

    DebugLogger* old3 = DebugLogger::swapGlobalDebugLogger(nullptr);
    EXPECT_EQ(old3, &log2);

    EXPECT_EQ(os.str(), "Error: Foo 0\nDebug: Foo 1\nDebug: Foo 2\n");
}
