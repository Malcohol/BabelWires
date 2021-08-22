#include <gtest/gtest.h>

#include "Common/BlockStream/streamEventHolder.hpp"

namespace {
    struct TestEvent : babelwires::StreamEvent {
        STREAM_EVENT(TestEvent);
        TestEvent(int v)
            : m_value(v) {}

        int m_value = 0;
    };
} // namespace

TEST(StreamEventHolder, Simple) {
    babelwires::StreamEventHolder<TestEvent> holder;

    EXPECT_FALSE(holder.hasEvent());

    TestEvent event(10);
    holder = event;
    EXPECT_TRUE(holder.hasEvent());
    EXPECT_EQ((*holder).m_value, 10);
    EXPECT_EQ(holder->m_value, 10);

    babelwires::StreamEventHolder<TestEvent> holder1 = TestEvent(15);
    EXPECT_TRUE(holder1.hasEvent());
    EXPECT_EQ(holder1->m_value, 15);

    holder.reset();
    EXPECT_FALSE(holder.hasEvent());

    holder = holder1;
    EXPECT_TRUE(holder.hasEvent());
    EXPECT_EQ(holder->m_value, 15);

    holder.reset();
    holder = babelwires::StreamEventHolder<TestEvent>(TestEvent(11));
    EXPECT_TRUE(holder.hasEvent());
    EXPECT_EQ(holder->m_value, 11);
}

namespace {
    struct alignas(16) BigAlignedEvent : TestEvent {
        STREAM_EVENT(BigAlignedEvent);
        BigAlignedEvent(int v)
            : TestEvent(v) {
            m_big[4] = v;
        }

        std::uint64_t m_big[5] = {};
    };
} // namespace

TEST(StreamEventHolder, Alignment) {
    babelwires::StreamEventHolder<TestEvent> holder;

    EXPECT_FALSE(holder.hasEvent());

    BigAlignedEvent event(10);
    holder = event;
    EXPECT_TRUE(holder.hasEvent());
    EXPECT_EQ((*holder).m_value, 10);
    EXPECT_EQ(holder->m_value, 10);
    ASSERT_NE(holder->as<BigAlignedEvent>(), nullptr);
    EXPECT_EQ(holder->as<BigAlignedEvent>()->m_value, 10);

    BigAlignedEvent event2(14);
    holder = event2;
    EXPECT_TRUE(holder.hasEvent());
    EXPECT_EQ((*holder).m_value, 14);
    EXPECT_EQ(holder->m_value, 14);
    ASSERT_NE(holder->as<BigAlignedEvent>(), nullptr);
    EXPECT_EQ(holder->as<BigAlignedEvent>()->m_value, 14);
}

namespace {
    struct Payload {
        Payload(int& payloadConstructionCounter, int& payloadDestructionCounter)
            : m_payloadConstructionCounter(payloadConstructionCounter)
            , m_payloadDestructionCounter(payloadDestructionCounter) {
            ++m_payloadConstructionCounter;
        }

        Payload(const Payload& other)
            : m_payloadConstructionCounter(other.m_payloadConstructionCounter)
            , m_payloadDestructionCounter(other.m_payloadDestructionCounter) {
            ++m_payloadConstructionCounter;
        }

        Payload(Payload&& other)
            : m_payloadConstructionCounter(other.m_payloadConstructionCounter)
            , m_payloadDestructionCounter(other.m_payloadDestructionCounter) {
            ++m_payloadConstructionCounter;
        }

        ~Payload() { ++m_payloadDestructionCounter; }

        int& m_payloadConstructionCounter;
        int& m_payloadDestructionCounter;
    };

    struct EventWithPayload : babelwires::StreamEvent {
        STREAM_EVENT(EventWithPayload);

        EventWithPayload(int& payloadConstructionCounter, int& payloadDestructionCounter)
            : m_payload(std::make_unique<Payload>(payloadConstructionCounter, payloadDestructionCounter)) {}

        EventWithPayload(EventWithPayload&& other)
            : StreamEvent(other) {
            m_payload = std::move(other.m_payload);
        }

        EventWithPayload(const EventWithPayload& other)
            : babelwires::StreamEvent(other)
            , m_payload(std::make_unique<Payload>(*other.m_payload)) {}

        ~EventWithPayload() { m_payload.reset(); }

        // In real examples, a shared_ptr to an immutable value could be a good choice.
        std::unique_ptr<Payload> m_payload;
    };
} // namespace

TEST(StreamEventHolder, PayloadTest) {
    int payloadConstructionCounter = 0;
    int payloadDestructionCounter = 0;
    {
        babelwires::StreamEventHolder<EventWithPayload> holder;

        {
            EventWithPayload event(payloadConstructionCounter, payloadDestructionCounter);
            EXPECT_EQ(payloadConstructionCounter, 1);
            EXPECT_EQ(payloadDestructionCounter, 0);
            holder = event;
            EXPECT_EQ(payloadConstructionCounter, 2);
            EXPECT_EQ(payloadDestructionCounter, 0);
        }
        EXPECT_EQ(payloadConstructionCounter, 2);
        EXPECT_EQ(payloadDestructionCounter, 1);

        babelwires::StreamEventHolder<EventWithPayload> holder1;
        holder1 = holder;
        EXPECT_EQ(payloadConstructionCounter, 3);
        EXPECT_EQ(payloadDestructionCounter, 1);

        holder1.reset();
        EXPECT_EQ(payloadConstructionCounter, 3);
        EXPECT_EQ(payloadDestructionCounter, 2);

        holder1 = std::move(holder);
        EXPECT_EQ(payloadConstructionCounter, 3);
        EXPECT_EQ(payloadDestructionCounter, 2);
    }
    EXPECT_EQ(payloadConstructionCounter, 3);
    EXPECT_EQ(payloadDestructionCounter, 3);

    payloadConstructionCounter = 0;
    payloadDestructionCounter = 0;
    {
        // Move from event
        babelwires::StreamEventHolder<EventWithPayload> holder(
            EventWithPayload(payloadConstructionCounter, payloadDestructionCounter));
        EXPECT_EQ(payloadConstructionCounter, 1);
        EXPECT_EQ(payloadDestructionCounter, 0);
    }
    EXPECT_EQ(payloadConstructionCounter, 1);
    EXPECT_EQ(payloadDestructionCounter, 1);

    payloadConstructionCounter = 0;
    payloadDestructionCounter = 0;
    {
        // Move constructor
        babelwires::StreamEventHolder<EventWithPayload> holder(babelwires::StreamEventHolder<EventWithPayload>(
            EventWithPayload(payloadConstructionCounter, payloadDestructionCounter)));
        EXPECT_EQ(payloadConstructionCounter, 1);
        EXPECT_EQ(payloadDestructionCounter, 0);
    }
    EXPECT_EQ(payloadConstructionCounter, 1);
    EXPECT_EQ(payloadDestructionCounter, 1);
}
