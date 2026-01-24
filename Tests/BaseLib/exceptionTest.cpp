#include <BaseLib/exceptions.hpp>

#include <gtest/gtest.h>

using namespace babelwires;

namespace {
    class TestException : public ExceptionWithStream<TestException> {};
} // namespace

TEST(ExceptionTest, exceptionTest) {
    try {
        try {
            throw TestException() << "Test streamability " << 15;
        } catch (TestException& e) {
            e << " with extra context " << 1.5;
            throw;
        }
    } catch (const TestException& e) {
        EXPECT_EQ(std::string(e.what()), "Test streamability 15 with extra context 1.5");
    }
}
