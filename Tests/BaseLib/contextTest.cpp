#include <gtest/gtest.h>

#include <BaseLib/Context/context.hpp>

namespace {
    struct ServiceA {
        int value = 42;
    };

    struct ServiceB {
        std::string name = "hello";
    };

    struct ServiceC {
        double data = 3.14;
    };
} // namespace

TEST(ContextTest, registerAndRetrieve) {
    babelwires::Context context;
    ServiceA a;
    ServiceB b;

    context.registerService<ServiceA>(a);
    context.registerService<ServiceB>(b);

    EXPECT_EQ(&context.get<ServiceA>(), &a);
    EXPECT_EQ(&context.get<ServiceB>(), &b);
    EXPECT_EQ(context.get<ServiceA>().value, 42);
    EXPECT_EQ(context.get<ServiceB>().name, "hello");
}

TEST(ContextTest, constAccess) {
    babelwires::Context context;
    ServiceA a;
    a.value = 99;

    context.registerService<ServiceA>(a);

    const babelwires::Context& constContext = context;
    EXPECT_EQ(&constContext.get<ServiceA>(), &a);
    EXPECT_EQ(constContext.get<ServiceA>().value, 99);
}

TEST(ContextTest, mutateViaGetService) {
    babelwires::Context context;
    ServiceA a;

    context.registerService<ServiceA>(a);
    context.get<ServiceA>().value = 100;
    EXPECT_EQ(a.value, 100);
}

TEST(ContextTest, independentInstances) {
    babelwires::Context context1;
    babelwires::Context context2;

    ServiceA a1;
    a1.value = 1;
    ServiceA a2;
    a2.value = 2;

    context1.registerService<ServiceA>(a1);
    context2.registerService<ServiceA>(a2);

    EXPECT_EQ(context1.get<ServiceA>().value, 1);
    EXPECT_EQ(context2.get<ServiceA>().value, 2);
    EXPECT_NE(&context1.get<ServiceA>(), &context2.get<ServiceA>());
}

TEST(ContextTest, getServiceAssertOnMissing) {
    babelwires::Context context;
    EXPECT_DEATH(context.get<ServiceA>(), "Service not registered");
}

TEST(ContextTest, registerServiceAssertOnDuplicate) {
    babelwires::Context context;
    ServiceA a1;
    ServiceA a2;
    context.registerService<ServiceA>(a1);
    EXPECT_DEATH(context.registerService<ServiceA>(a2), "Service already registered");
}
