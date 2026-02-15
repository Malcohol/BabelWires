#include <gtest/gtest.h>

#include <BaseLib/Utilities/result.hpp>

namespace {
    const char expectedErrorPrefix[] = "An error occurred";

    babelwires::Result functionThatReturnsSuccess() {
        return {};
    }

    babelwires::Result functionThatReturnsError() {
        return babelwires::Error() << expectedErrorPrefix << " with code " << 123;
    }

    babelwires::ResultT<int> functionThatReturnsSuccessWithValue() {
        return 42;
    }

    babelwires::ResultT<int> functionThatReturnsErrorWithValue() {
        return babelwires::Error() << expectedErrorPrefix << " with code " << 123;
    }
} // namespace

TEST(ResultTest, resultSuccess) {
    const babelwires::Result result = functionThatReturnsSuccess();
    EXPECT_TRUE(result);
}

TEST(ResultTest, resultError) {
    const babelwires::Result result = functionThatReturnsError();
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().toString().find(expectedErrorPrefix), 0);
}

TEST(ResultTest, resultTSuccess) {
    babelwires::ResultT<int> result = functionThatReturnsSuccessWithValue();
    EXPECT_TRUE(result);
    EXPECT_EQ(result.value(), 42);
}

TEST(ResultTest, resultTError) {
    babelwires::ResultT<int> result = functionThatReturnsErrorWithValue();
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().toString().find(expectedErrorPrefix), 0);
}

TEST(ResultTest, compositeFunctions) {
    auto functionThatCallsOtherFunction = [](bool& afterSuccess, bool& afterError) -> babelwires::Result {
        DO_OR_ERROR(functionThatReturnsSuccess());
        afterSuccess = true;
        DO_OR_ERROR(functionThatReturnsError());
        afterError = true;
        return {};
    };

    bool afterSuccess = false;
    bool afterError = false;
    const babelwires::Result result = functionThatCallsOtherFunction(afterSuccess, afterError);
    EXPECT_FALSE(result);
    EXPECT_TRUE(afterSuccess);
    EXPECT_FALSE(afterError);
}

TEST(ResultTest, onErrorMacroWithNestedOnError) {
    auto functionThatCallsOtherFunctionWithOnError = [](bool& afterSuccess, bool& afterError,
                                                        bool& onErrorCalled, bool& onErrorCalled2) -> babelwires::Result {
        ON_ERROR(onErrorCalled = true);
        {
            ON_ERROR(onErrorCalled2 = true);
            DO_OR_ERROR(functionThatReturnsSuccess());
            afterSuccess = true;
            DO_OR_ERROR(functionThatReturnsError());
            afterError = true;
            return {};
        }
    };

    bool afterSuccess = false;
    bool afterError = false;
    bool onErrorCalled = false;
    bool onErrorCalled2 = false;
    const babelwires::Result result =
        functionThatCallsOtherFunctionWithOnError(afterSuccess, afterError, onErrorCalled, onErrorCalled2);
    EXPECT_FALSE(result);
    EXPECT_TRUE(afterSuccess);
    EXPECT_FALSE(afterError);
    EXPECT_TRUE(onErrorCalled);
    EXPECT_TRUE(onErrorCalled2);
}

TEST(ResultTest, onErrorMacroWithoutError) {
    auto functionWithOnErrorButNoError = [](bool& onErrorCalled) -> babelwires::Result {
        ON_ERROR(onErrorCalled = true);
        DO_OR_ERROR(functionThatReturnsSuccess());
        return {};
    };

    bool onErrorCalled = false;
    const babelwires::Result result = functionWithOnErrorButNoError(onErrorCalled);
    EXPECT_TRUE(result);
    EXPECT_FALSE(onErrorCalled);
}

TEST(ResultTest, assignOrError) {
    auto functionUsingAssignOrError = [](int& successValue, int& errorValue) -> babelwires::Result {
        ASSIGN_OR_ERROR(successValue, functionThatReturnsSuccessWithValue());
        ASSIGN_OR_ERROR(errorValue, functionThatReturnsErrorWithValue());
        return {};
    };

    int successValue = 0;
    int errorValue = 0;
    const babelwires::Result result = functionUsingAssignOrError(successValue, errorValue);
    EXPECT_FALSE(result);
    EXPECT_EQ(successValue, 42);
    EXPECT_EQ(errorValue, 0);
}

TEST(ResultTest, assignOrError2) {
    auto functionUsingAssignOrError2 = [](int& successValue, int& errorValue) -> babelwires::Result {
        ASSIGN_OR_ERROR(int x, functionThatReturnsSuccessWithValue());
        successValue = x;
        ASSIGN_OR_ERROR(int y, functionThatReturnsErrorWithValue());
        errorValue = y;
        return {};
    };

    int successValue = 0;
    int errorValue = 0;
    const babelwires::Result result = functionUsingAssignOrError2(successValue, errorValue);
    EXPECT_FALSE(result);
    EXPECT_EQ(successValue, 42);
    EXPECT_EQ(errorValue, 0);
}

TEST(ResultTest, assignOrErrorWithOnError) {
    auto functionUsingAssignOrErrorWithOnError = [](int& successValue, int& errorValue, bool& onErrorCalled) -> babelwires::Result {
        ON_ERROR(onErrorCalled = true);
        ASSIGN_OR_ERROR(successValue, functionThatReturnsSuccessWithValue());
        ASSIGN_OR_ERROR(errorValue, functionThatReturnsErrorWithValue());
        return {};
    };

    int successValue = 0;
    int errorValue = 0;
    bool onErrorCalled = false;
    const babelwires::Result result = functionUsingAssignOrErrorWithOnError(successValue, errorValue, onErrorCalled);
    EXPECT_FALSE(result);
    EXPECT_EQ(successValue, 42);
    EXPECT_EQ(errorValue, 0);
    EXPECT_TRUE(onErrorCalled);
}

TEST(ResultTest, finallyWithSuccess) {
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

TEST(ResultTest, finallyWithError) {
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

TEST(ResultTest, finallyWithErrorStateWithSuccess) {
    auto functionThatUsesFinallyWithErrorStateWithSuccess = [](bool& wasFinallyRun, babelwires::ErrorState& capturedErrorState) -> babelwires::Result {
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

TEST(ResultTest, finallyWithErrorStateWithError) {
    auto functionThatUsesFinallyWithErrorStateWithError = [](bool& wasFinallyRun, babelwires::ErrorState& capturedErrorState) -> babelwires::Result {
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