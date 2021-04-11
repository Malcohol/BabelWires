#include "Common/Signal/signal.hpp"
#include <gtest/gtest.h>

namespace {
    void staticSlot(int x, unsigned int* count, int* sum) {
        ++(*count);
        *sum += x;
    }
} // namespace

TEST(Signal, functionality) {
    // The second two slots allow the staticSlot to produce observable effects
    // without relying on global variables.
    babelwires::Signal<int, unsigned int*, int*> m_signal;

    unsigned int staticCount = 0;
    int staticSum = 0;

    m_signal.fire(1, &staticCount, &staticSum);

    unsigned int lambdaCount = 0;
    int lambdaSum = 0;

    auto lambdaSlot = [&lambdaCount, &lambdaSum](int x, unsigned int*, int*) {
        ++lambdaCount;
        lambdaSum += x;
    };

    struct Object {
        void objectSlot(int x, unsigned int*, int*) {
            ++m_count;
            m_sum += x;
        }

        unsigned int m_count = 0;
        int m_sum = 0;
    };

    Object object;

    babelwires::SignalSubscription staticSubscription = m_signal.subscribe(staticSlot);
    m_signal.fire(2, &staticCount, &staticSum);

    EXPECT_EQ(staticCount, 1);
    EXPECT_EQ(staticSum, 2);

    {
        babelwires::SignalSubscription lambdaSubscription = m_signal.subscribe(lambdaSlot);
        m_signal.fire(4, &staticCount, &staticSum);

        EXPECT_EQ(staticCount, 2);
        EXPECT_EQ(staticSum, 2 + 4);
        EXPECT_EQ(lambdaCount, 1);
        EXPECT_EQ(lambdaSum, 4);

        staticSubscription.unsubscribe();
        m_signal.fire(8, &staticCount, &staticSum);

        EXPECT_EQ(staticCount, 2);
        EXPECT_EQ(staticSum, 2 + 4);
        EXPECT_EQ(lambdaCount, 2);
        EXPECT_EQ(lambdaSum, 4 + 8);
    }
    m_signal.fire(16, &staticCount, &staticSum);

    EXPECT_EQ(staticCount, 2);
    EXPECT_EQ(staticSum, 2 + 4);
    EXPECT_EQ(lambdaCount, 2);
    EXPECT_EQ(lambdaSum, 4 + 8);

    babelwires::SignalSubscription objectSubscription = m_signal.subscribe(&object, &Object::objectSlot);
    m_signal.fire(32, &staticCount, &staticSum);

    EXPECT_EQ(staticCount, 2);
    EXPECT_EQ(staticSum, 2 + 4);
    EXPECT_EQ(lambdaCount, 2);
    EXPECT_EQ(lambdaSum, 4 + 8);
    EXPECT_EQ(object.m_count, 1);
    EXPECT_EQ(object.m_sum, 32);
}