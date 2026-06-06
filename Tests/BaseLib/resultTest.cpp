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

    babelwires::ResultT<int> functionThatReturnsSuccessWithValue() {
        return 42;
    }

    babelwires::ResultT<int> functionThatReturnsErrorWithValue() {
        return babelwires::Error() << expectedErrorPrefix << " with code " << 234;
    }

    babelwires::ResultT<std::tuple<int, int, int>> functionThatReturnsSuccessfulTuple() {
        return std::tuple{1, 2, 3};
    }

    babelwires::ResultT<std::tuple<int, int, int>> functionThatReturnsFailureTuple() {
        return babelwires::Error() << expectedErrorPrefix << " with code " << 345;
    }

    struct Foo { 
        int x = 0; 
        void setX(int newX) { x = newX; }
    };

    babelwires::ResultT<Foo&> functionThatReturnsSuccessWithReference(Foo& foo) {
        return foo;
    }

    babelwires::ResultT<Foo&> functionThatReturnsErrorWithReference(Foo& /*foo*/) {
        return babelwires::Error() << expectedErrorPrefix << " with code " << 456;
    }

        babelwires::ResultT<const Foo&> functionThatReturnsSuccessWithConstReference(const Foo& foo) {
        return foo;
    }

    babelwires::ResultT<const Foo&> functionThatReturnsErrorWithConstReference(const Foo& /*foo*/) {
        return babelwires::Error() << expectedErrorPrefix << " with code " << 456;
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
    auto functionThatCallsOtherFunctionWithOnError = [](bool& afterSuccess, bool& afterError, bool& onErrorCalled,
                                                        bool& onErrorCalled2) -> babelwires::Result {
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
    auto functionUsingAssignOrErrorWithOnError = [](int& successValue, int& errorValue,
                                                    bool& onErrorCalled) -> babelwires::Result {
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

TEST(ResultTest, assignOrErrorWithStructuredBinding) {
    auto functionUsingStructuredBinding = []() -> babelwires::Result {
        ASSIGN_OR_ERROR(auto [a, b, c], functionThatReturnsSuccessfulTuple());
        EXPECT_EQ(a, 1);
        EXPECT_EQ(b, 2);
        EXPECT_EQ(c, 3);
        return {};
    };

    const babelwires::Result result = functionUsingStructuredBinding();
    EXPECT_TRUE(result);
}

TEST(ResultTest, assignOrErrorWithStructuredBindingError) {
    auto functionUsingStructuredBinding = [](bool& afterAssign, bool& onErrorCalled) -> babelwires::Result {
        ON_ERROR(onErrorCalled = true);
        ASSIGN_OR_ERROR(auto [a, b, c], functionThatReturnsFailureTuple());
        afterAssign = true;
        return {};
    };

    bool afterAssign = false;
    bool onErrorCalled = false;
    const babelwires::Result result = functionUsingStructuredBinding(afterAssign, onErrorCalled);
    EXPECT_FALSE(result);
    EXPECT_FALSE(afterAssign);
    EXPECT_TRUE(onErrorCalled);
    EXPECT_EQ(result.error().toString().find(expectedErrorPrefix), 0);
}

TEST(ResultTest, assignOrErrorWithReference) {
    auto functionUsingAssignOrErrorWithReference = [](Foo& foo, int& successValue, int& errorValue,
                                                     bool& onErrorCalled) -> babelwires::Result {
        ON_ERROR(onErrorCalled = true);
        ASSIGN_OR_ERROR(Foo& val0, functionThatReturnsSuccessWithReference(foo));
        successValue = val0.x;
        val0.setX(100);
        ASSIGN_OR_ERROR(Foo& val1, functionThatReturnsErrorWithReference(foo));
        errorValue = val1.x;
        return {};
    };

    Foo foo{42};
    int successValue = 0;
    int errorValue = 0;
    bool onErrorCalled = false;
    const babelwires::Result result =
        functionUsingAssignOrErrorWithReference(foo, successValue, errorValue, onErrorCalled);
    EXPECT_FALSE(result);
    EXPECT_EQ(successValue, 42);
    EXPECT_EQ(errorValue, 0);
    EXPECT_EQ(foo.x, 100);
    EXPECT_TRUE(onErrorCalled);
}

TEST(ResultTest, assignOrErrorWithConstReference) {
    auto functionUsingAssignOrErrorWithConstReference = [](const Foo& foo, int& successValue, int& errorValue,
                                                           bool& onErrorCalled) -> babelwires::Result {
        ON_ERROR(onErrorCalled = true);
        ASSIGN_OR_ERROR(const Foo& val0, functionThatReturnsSuccessWithConstReference(foo));
        successValue = val0.x;
        ASSIGN_OR_ERROR(const Foo& val1, functionThatReturnsErrorWithConstReference(foo));
        errorValue = val1.x;
        return {};
    };

    const Foo foo{42};
    int successValue = 0;
    int errorValue = 0;
    bool onErrorCalled = false;
    const babelwires::Result result =
        functionUsingAssignOrErrorWithConstReference(foo, successValue, errorValue, onErrorCalled);
    EXPECT_FALSE(result);
    EXPECT_EQ(successValue, 42);
    EXPECT_EQ(errorValue, 0);
    EXPECT_TRUE(onErrorCalled);
}

TEST(ResultTest, returnError) {
    auto functionUsingReturnErrorMacro = [](bool& onErrorCalled) -> babelwires::Result {
        ON_ERROR(onErrorCalled = true);
        RETURN_ERROR() << "Test";
    };

    bool onErrorCalled = false;
    const babelwires::Result result = functionUsingReturnErrorMacro(onErrorCalled);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().toString(), "Test");
    EXPECT_TRUE(onErrorCalled);
}

TEST(ResultTest, assertNoError) {
    ASSERT_NO_ERROR(functionThatReturnsSuccess());
    const int i = ASSERT_NO_ERROR(functionThatReturnsSuccessWithValue());
    EXPECT_EQ(i, 42);
}
