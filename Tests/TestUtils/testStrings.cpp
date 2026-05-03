#include <Tests/TestUtils/testStrings.hpp>

/// Try to prevent the compiler from inlining.
#if defined(_MSC_VER)
#define BABELWIRES_TEST_NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define BABELWIRES_TEST_NOINLINE __attribute__((noinline))
#else
#define BABELWIRES_TEST_NOINLINE
#endif

BABELWIRES_TEST_NOINLINE std::string_view testUtils::getTestStringOutOfLine() {
    return c_testString;
}

