#include <Common/Cloning/cloneable.hpp>
#include <gtest/gtest.h>

#include <algorithm>

using namespace babelwires;

namespace {
    struct A : public Cloneable {};

    struct B : public A {
        CLONEABLE_ABSTRACT(B);
        B(int x)
            : m_x(x) {}

        virtual int foo() const = 0;

        int m_x;
    };

    struct C : public B {
        C(int x, int y)
            : B(x)
            , m_y(y) {}
        CLONEABLE(C);
        int foo() const override { return m_y; }

        int m_y;
    };
} // namespace

TEST(CloneTest, basic) {
    C c(10, 11);
    EXPECT_EQ(c.m_x, 10);
    EXPECT_EQ(c.m_y, 11);
    {
        std::unique_ptr<C> clone = c.clone();
        EXPECT_EQ(c.m_x, clone->m_x);
        EXPECT_EQ(c.m_y, clone->m_y);
    }
    {
        std::unique_ptr<B> clone = static_cast<B&>(c).clone();
        EXPECT_EQ(c.m_x, clone->m_x);
        const C* cloneAsC = dynamic_cast<C*>(clone.get());
        EXPECT_NE(cloneAsC, nullptr);
        EXPECT_EQ(c.m_y, cloneAsC->m_y);
    }
}

#include <iostream>

namespace {
    struct M : public Cloneable {
        CLONEABLE(M);
        M(int x) : m_x(x) { m_state = Constructed; }
        M(const M& other) : m_x(other.m_x) { m_state = Copied; }
        M(M&& other) : m_x(other.m_x) { m_state = Moved; }

        int m_x = 0;
        enum State { Constructed, Copied, Moved } m_state;
    };
}

TEST(CloneTest, move) {
    M m(12);
    auto copiedClone = m.clone();
    EXPECT_EQ(copiedClone->m_state, M::Copied);
    auto movedClone = std::move(m).clone();
    EXPECT_EQ(movedClone->m_state, M::Moved);
}

namespace {
    struct CloneContext;

    struct Patchable {
        virtual bool postClonePatching(const CloneContext& context) = 0;
    };

    struct CloneContext {
        std::unordered_map<const void*, void*> m_cloneMap;
        std::vector<Patchable*> m_objectsToPatch;
        void patch() {
            std::for_each(m_objectsToPatch.begin(), m_objectsToPatch.end(),
                          [this](Patchable* p) { p->postClonePatching(*this); });
        }
    };

    struct X : public CustomCloneable<CloneContext> {
        CUSTOM_CLONEABLE(X);

        X() = default;

        X(const X& other, CloneContext& context) {
            context.m_cloneMap.insert(std::pair(&other, this));
            if (other.m_leftChild) {
                m_leftChild = other.m_leftChild->customClone(context);
            }
            if (other.m_rightChild) {
                m_rightChild = other.m_rightChild->customClone(context);
            }
            if (other.m_parent) {
                const auto it = context.m_cloneMap.find(other.m_parent);
                if (it != context.m_cloneMap.end()) {
                    m_parent = static_cast<const X*>(it->second);
                } else {
                    m_parent = other.m_parent;
                }
            }
        }
        std::unique_ptr<X> m_leftChild = nullptr;
        std::unique_ptr<X> m_rightChild = nullptr;
        const X* m_parent = nullptr;
    };

    struct Y : public X, Patchable {
        CUSTOM_CLONEABLE(Y);

        Y() = default;

        Y(const Y& other, CloneContext& context)
            : X(other, context) {
            m_distantRelation = other.m_distantRelation;
            if (!postClonePatching(context)) {
                context.m_objectsToPatch.emplace_back(this);
            }
        }

        bool postClonePatching(const CloneContext& context) override {
            if (m_distantRelation) {
                const auto it = context.m_cloneMap.find(m_distantRelation);
                if (it != context.m_cloneMap.end()) {
                    m_distantRelation = static_cast<const X*>(it->second);
                    return true;
                }
            }
            return false;
        }

        const X* m_distantRelation = nullptr;
    };

    std::unique_ptr<X> graphClone(const X* root) {
        CloneContext context;
        std::unique_ptr<X> clone = root->customClone(context);
        context.patch();
        return clone;
    }

    std::unique_ptr<X> createGraph() {
        std::unique_ptr<X> root = std::make_unique<X>();
        root->m_leftChild = std::make_unique<X>();
        root->m_rightChild = std::make_unique<X>();
        root->m_leftChild->m_leftChild = std::make_unique<Y>();
        root->m_rightChild->m_rightChild = std::make_unique<Y>();

        root->m_leftChild->m_parent = root.get();
        root->m_rightChild->m_parent = root.get();
        root->m_leftChild->m_leftChild->m_parent = root->m_leftChild.get();
        root->m_rightChild->m_rightChild->m_parent = root->m_rightChild.get();

        dynamic_cast<Y*>(root->m_leftChild->m_leftChild.get())->m_distantRelation =
            root->m_rightChild->m_rightChild.get();
        dynamic_cast<Y*>(root->m_rightChild->m_rightChild.get())->m_distantRelation =
            root->m_leftChild->m_leftChild.get();

        return root;
    }

} // namespace

TEST(CloneTest, customGraphCloningRoot) {
    std::unique_ptr<X> root = createGraph();
    std::unique_ptr<X> clone = graphClone(root.get());

    EXPECT_NE(clone, nullptr);
    EXPECT_NE(clone.get(), root.get());
    EXPECT_NE(clone->m_leftChild.get(), nullptr);
    EXPECT_NE(clone->m_rightChild.get(), nullptr);
    EXPECT_NE(clone->m_leftChild->m_leftChild.get(), nullptr);
    EXPECT_NE(clone->m_rightChild->m_rightChild.get(), nullptr);

    EXPECT_EQ(clone->m_leftChild->m_parent, clone.get());
    EXPECT_EQ(clone->m_rightChild->m_parent, clone.get());
    EXPECT_EQ(clone->m_leftChild->m_leftChild->m_parent, clone->m_leftChild.get());
    EXPECT_EQ(clone->m_rightChild->m_rightChild->m_parent, clone->m_rightChild.get());

    EXPECT_NE(dynamic_cast<Y*>(clone->m_leftChild->m_leftChild.get()), nullptr);
    EXPECT_NE(dynamic_cast<Y*>(clone->m_rightChild->m_rightChild.get()), nullptr);

    EXPECT_EQ(dynamic_cast<Y*>(clone->m_leftChild->m_leftChild.get())->m_distantRelation,
              clone->m_rightChild->m_rightChild.get());
    EXPECT_EQ(dynamic_cast<Y*>(clone->m_rightChild->m_rightChild.get())->m_distantRelation,
              clone->m_leftChild->m_leftChild.get());
}

TEST(CloneTest, customGraphCloningSubgraph) {
    std::unique_ptr<X> root = createGraph();
    std::unique_ptr<X> clone = graphClone(root->m_leftChild.get());

    EXPECT_NE(clone, nullptr);
    EXPECT_NE(clone.get(), root.get());
    EXPECT_NE(clone->m_leftChild.get(), nullptr);

    EXPECT_EQ(clone->m_parent, root.get());
    EXPECT_EQ(clone->m_leftChild->m_parent, clone.get());
    EXPECT_NE(dynamic_cast<Y*>(clone->m_leftChild.get()), nullptr);
    EXPECT_EQ(dynamic_cast<Y*>(clone->m_leftChild.get())->m_distantRelation, root->m_rightChild->m_rightChild.get());
}
