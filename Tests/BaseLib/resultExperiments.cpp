#include <gtest/gtest.h>

#include <BaseLib/Result/resultDSL.hpp>

namespace {
    const char expectedErrorPrefix[] = "An error occurred";

    babelwires::Result functionThatReturnsSuccess() {
        return {};
    }

    babelwires::Result functionThatReturnsError() {
        return babelwires::Error() << expectedErrorPrefix << " with code " << 123;
    }
}

// Experimental macro allowing the caller to disable the last ON_ERROR handler.
// It requires a new scope and can only be used for the innermost ON_ERROR, so it's
// not very ergonomic.
#define DISABLE_LAST_ON_ERROR() const auto babelwiresOnError = babelwiresOnError2;

TEST(ResultExperiment, disableTest) {
    auto functionThatDisablesOnError = [](bool& onErrorCalled) -> babelwires::Result {
        ON_ERROR(onErrorCalled = true);
        {
            DISABLE_LAST_ON_ERROR();
            DO_OR_ERROR(functionThatReturnsError());
            return {};
        }
    };

    bool onErrorCalled = false;
    const babelwires::Result result = functionThatDisablesOnError(onErrorCalled);
    EXPECT_FALSE(result);
    EXPECT_FALSE(onErrorCalled);
}

// This code has been removed from result.hpp while I decide whether it's helpful or harmful.
// This concern is that the code passed to FINALLY_WITH_ERRORSTATE cannot itself return an error.
// Code that ends up in finally clauses (e.g. close methods) often does encounter an error that (if no error has
// already occurred) should be passed back to the caller.

namespace babelwires {
    /// Common functionality.
    enum class ErrorState { NoError, Error };

    namespace Detail {
        /// Helper class for running code at the end of a scope.
        template <typename F> struct Finally {
            F m_func;
            explicit Finally(F f)
                : m_func(std::move(f)) {}
            ~Finally() { m_func(); }
            Finally(const Finally&) = delete;
            Finally& operator=(const Finally&) = delete;
        };
    } // namespace Detail
} // namespace babelwires

/// Run the code at the end of the scope.
#define FINALLY(CODE_TO_RUN)                                                                                           \
    const auto BW_UNIQUE_NAME(babelwiresFinally_, __LINE__) = ::babelwires::Detail::Finally([&]() { CODE_TO_RUN; });

/// Run the code at the end of the scope. The code can query a variable called errorState to determine whether the scope
/// is being exited due to an error or not. Since this uses ON_ERROR, it cannot be used in a scope that already uses
/// ON_ERROR or FINALLY_WITH_ERRORSTATE, but you can introduce a new scope to work around that.
#define FINALLY_WITH_ERRORSTATE(CODE_TO_RUN)                                                                           \
    babelwires::ErrorState BW_UNIQUE_NAME(babelwiresFinallyErrorState_, __LINE__) = babelwires::ErrorState::NoError;   \
    ON_ERROR(BW_UNIQUE_NAME(babelwiresFinallyErrorState_, __LINE__) = babelwires::ErrorState::Error);                  \
    const auto BW_UNIQUE_NAME(babelwiresFinally_, __LINE__) = ::babelwires::Detail::Finally(                           \
        [&, &errorState = BW_UNIQUE_NAME(babelwiresFinallyErrorState_, __LINE__)]() { CODE_TO_RUN; });

TEST(ResultExperiment, finallyWithSuccess) {
    auto functionThatUsesFinallyWithSuccess = [](bool& wasFinallyRun) -> babelwires::Result {
        FINALLY(wasFinallyRun = true);
        DO_OR_ERROR(functionThatReturnsSuccess());
        return {};
    };

    bool wasFinallyRun = false;
    const babelwires::Result result = functionThatUsesFinallyWithSuccess(wasFinallyRun);
    EXPECT_TRUE(result);
    EXPECT_TRUE(wasFinallyRun);
}

TEST(ResultExperiment, finallyWithError) {
    auto functionThatUsesFinallyWithError = [](bool& wasFinallyRun) -> babelwires::Result {
        FINALLY(wasFinallyRun = true);
        DO_OR_ERROR(functionThatReturnsError());
        return {};
    };

    bool wasFinallyRun = false;
    const babelwires::Result result = functionThatUsesFinallyWithError(wasFinallyRun);
    EXPECT_FALSE(result);
    EXPECT_TRUE(wasFinallyRun);
}

TEST(ResultExperiment, finallyWithErrorStateWithSuccess) {
    auto functionThatUsesFinallyWithErrorStateWithSuccess =
        [](bool& wasFinallyRun, babelwires::ErrorState& capturedErrorState) -> babelwires::Result {
        FINALLY_WITH_ERRORSTATE(wasFinallyRun = true; capturedErrorState = errorState);
        DO_OR_ERROR(functionThatReturnsSuccess());
        return {};
    };

    bool wasFinallyRun = false;
    babelwires::ErrorState capturedErrorState = babelwires::ErrorState::NoError;
    const babelwires::Result result =
        functionThatUsesFinallyWithErrorStateWithSuccess(wasFinallyRun, capturedErrorState);
    EXPECT_TRUE(result);
    EXPECT_TRUE(wasFinallyRun);
    EXPECT_EQ(capturedErrorState, babelwires::ErrorState::NoError);
}

TEST(ResultExperiment, finallyWithErrorStateWithError) {
    auto functionThatUsesFinallyWithErrorStateWithError =
        [](bool& wasFinallyRun, babelwires::ErrorState& capturedErrorState) -> babelwires::Result {
        FINALLY_WITH_ERRORSTATE(wasFinallyRun = true; capturedErrorState = errorState);
        DO_OR_ERROR(functionThatReturnsError());
        return {};
    };

    bool wasFinallyRun = false;
    babelwires::ErrorState capturedErrorState = babelwires::ErrorState::NoError;
    const babelwires::Result result = functionThatUsesFinallyWithErrorStateWithError(wasFinallyRun, capturedErrorState);
    EXPECT_FALSE(result);
    EXPECT_TRUE(wasFinallyRun);
    EXPECT_EQ(capturedErrorState, babelwires::ErrorState::Error);
}
