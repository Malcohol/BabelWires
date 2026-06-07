#include <BaseLib/Utilities/interfaceProvider.hpp>

#include <gtest/gtest.h>

namespace {
    struct InterfaceA {
        QUERYABLE_INTERFACE(InterfaceA);
        virtual ~InterfaceA() = default;
        virtual int getA() const = 0;
    };

    struct InterfaceB {
        QUERYABLE_INTERFACE(InterfaceB);
        virtual ~InterfaceB() = default;
        virtual int getB() const = 0;
    };

    struct InterfaceC {
        QUERYABLE_INTERFACE(InterfaceC);
        virtual ~InterfaceC() = default;
        virtual int getC() const = 0;
    };

    struct Base {
        QUERYABLE_INTERFACE_PROVIDER_BASE();
        virtual ~Base() = default;
    };

    struct Derived : public Base, public InterfaceA, public InterfaceB {
        QUERYABLE_INTERFACE_PROVIDER(Base, InterfaceA, InterfaceB);

        int getA() const override { return 11; }
        int getB() const override { return 17; }
    };

    struct FurtherDerived : public Derived, public InterfaceC {
        QUERYABLE_INTERFACE_PROVIDER(Derived, InterfaceC);

        int getC() const override { return 23; }
    };
} // namespace

TEST(QueryableInterfacesTest, queryFromBaseReference) {
    Derived derived;
    Base& base = derived;

    InterfaceA* interfaceA = base.tryInterface<InterfaceA>();
    InterfaceB* interfaceB = base.tryInterface<InterfaceB>();
    InterfaceC* interfaceC = base.tryInterface<InterfaceC>();

    ASSERT_NE(interfaceA, nullptr);
    ASSERT_NE(interfaceB, nullptr);
    EXPECT_EQ(interfaceA->getA(), 11);
    EXPECT_EQ(interfaceB->getB(), 17);
    EXPECT_EQ(interfaceC, nullptr);
}

TEST(QueryableInterfacesTest, queryFromConstBaseReference) {
    const FurtherDerived derived;
    const Base& base = derived;

    const InterfaceA* interfaceA = base.tryInterface<InterfaceA>();
    const InterfaceB* interfaceB = base.tryInterface<InterfaceB>();
    const InterfaceC* interfaceC = base.tryInterface<InterfaceC>();

    ASSERT_NE(interfaceA, nullptr);
    ASSERT_NE(interfaceB, nullptr);
    ASSERT_NE(interfaceC, nullptr);
    EXPECT_EQ(interfaceA->getA(), 11);
    EXPECT_EQ(interfaceB->getB(), 17);
    EXPECT_EQ(interfaceC->getC(), 23);
}

TEST(QueryableInterfacesTest, queryFromDerivedParentChain) {
    FurtherDerived derived;
    Derived& parent = derived;

    InterfaceA* interfaceA = parent.tryInterface<InterfaceA>();
    InterfaceB* interfaceB = parent.tryInterface<InterfaceB>();
    InterfaceC* interfaceC = parent.tryInterface<InterfaceC>();

    ASSERT_NE(interfaceA, nullptr);
    ASSERT_NE(interfaceB, nullptr);
    ASSERT_NE(interfaceC, nullptr);
    EXPECT_EQ(interfaceA->getA(), 11);
    EXPECT_EQ(interfaceB->getB(), 17);
    EXPECT_EQ(interfaceC->getC(), 23);
}