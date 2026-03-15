#include <BaseLib/Utilities/downcastable.hpp>

#include <gtest/gtest.h>

namespace {
    struct Base {
        DOWNCASTABLE_BASE(Base)
    };

    struct Derived : public Base {
        DOWNCASTABLE(Derived, Base)
    };

    struct AnotherDerived : public Base {
        DOWNCASTABLE(AnotherDerived, Base)
    };

    struct GrandChild : public Derived {
        DOWNCASTABLE(GrandChild, Derived)
    };
} // namespace

TEST(DowncastableTest, downcasting) {
    Base base;
    Derived derived;
    AnotherDerived anotherDerived;
    GrandChild grandChild;

    // Test basic downcasting
    EXPECT_NE(base.tryAs<Base>(), nullptr);
    EXPECT_EQ(base.tryAs<Derived>(), nullptr);

    EXPECT_NE(derived.tryAs<Base>(), nullptr);
    EXPECT_NE(derived.tryAs<Derived>(), nullptr);
    EXPECT_EQ(derived.tryAs<AnotherDerived>(), nullptr);

    EXPECT_NE(anotherDerived.tryAs<Base>(), nullptr);
    EXPECT_EQ(anotherDerived.tryAs<Derived>(), nullptr);
    EXPECT_NE(anotherDerived.tryAs<AnotherDerived>(), nullptr);

    EXPECT_NE(grandChild.tryAs<Base>(), nullptr);
    EXPECT_NE(grandChild.tryAs<Derived>(), nullptr);
    EXPECT_EQ(grandChild.tryAs<AnotherDerived>(), nullptr);
    EXPECT_NE(grandChild.tryAs<GrandChild>(), nullptr);

    // Downcasting from base reference.
    Base& baseRef = derived;
    EXPECT_NE(baseRef.tryAs<Base>(), nullptr);
    EXPECT_NE(baseRef.tryAs<Derived>(), nullptr);
    EXPECT_EQ(baseRef.tryAs<AnotherDerived>(), nullptr);

    // Downcasting from base reference.
    Base& basePtr = grandChild;
    EXPECT_NE(basePtr.tryAs<Base>(), nullptr);
    EXPECT_NE(basePtr.tryAs<Derived>(), nullptr);
    EXPECT_EQ(basePtr.tryAs<AnotherDerived>(), nullptr);
    EXPECT_NE(basePtr.tryAs<GrandChild>(), nullptr);

    // These should not assert
    base.as<Base>();
    derived.as<Base>();
    derived.as<Derived>();
    anotherDerived.as<Base>();
    anotherDerived.as<AnotherDerived>();
    grandChild.as<Base>();
    grandChild.as<Derived>();
    grandChild.as<GrandChild>();
}

TEST(DowncastableTest, upcasting) {
    // Up-casting isn't strictly useful, since it's statically known whether the cast will succeed or not
    // and a static_cast can be used.
    // However, it's worth noting that it works with the current implementation.
    // This test could be removed if we decide to remove support for up-casting in the future.

    Base base;
    Derived derived;
    AnotherDerived anotherDerived;
    GrandChild grandChild;

    EXPECT_NE(derived.tryAs<Base>(), nullptr);
    EXPECT_NE(anotherDerived.tryAs<Base>(), nullptr);
    EXPECT_NE(grandChild.tryAs<Base>(), nullptr);
    EXPECT_NE(grandChild.tryAs<Derived>(), nullptr);
    EXPECT_EQ(grandChild.tryAs<AnotherDerived>(), nullptr);
}

TEST(DowncastableTest, getHierarchyId) {
    Base base;
    Derived derived;
    AnotherDerived anotherDerived;
    GrandChild grandChild;

    EXPECT_NE(Base::getHierarchyIdStatic(), Derived::getHierarchyIdStatic());
    EXPECT_NE(Base::getHierarchyIdStatic(), AnotherDerived::getHierarchyIdStatic());
    EXPECT_NE(Base::getHierarchyIdStatic(), GrandChild::getHierarchyIdStatic());
    EXPECT_NE(Derived::getHierarchyIdStatic(), AnotherDerived::getHierarchyIdStatic());
    EXPECT_NE(Derived::getHierarchyIdStatic(), GrandChild::getHierarchyIdStatic());
    EXPECT_NE(AnotherDerived::getHierarchyIdStatic(), GrandChild::getHierarchyIdStatic());

    EXPECT_EQ(base.getHierarchyId(), Base::getHierarchyIdStatic());
    EXPECT_EQ(derived.getHierarchyId(), Derived::getHierarchyIdStatic());
    EXPECT_EQ(anotherDerived.getHierarchyId(), AnotherDerived::getHierarchyIdStatic());
    EXPECT_EQ(grandChild.getHierarchyId(), GrandChild::getHierarchyIdStatic());
}
