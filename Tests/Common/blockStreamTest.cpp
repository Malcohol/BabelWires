#include <gtest/gtest.h>

#include "Common/BlockStream/blockStream.hpp"

TEST(BlockStream, Simple) {
    babelwires::BlockStream stream;

    EXPECT_TRUE(stream.getNumEvents() == 0);

    babelwires::StreamEvent event0;
    stream.addEvent(event0);

    EXPECT_TRUE(stream.getNumEvents() == 1);

    stream.addEvent(babelwires::StreamEvent());

    EXPECT_TRUE(stream.getNumEvents() == 2);

    int count = 0;
    for (const auto& event : stream) {
        ++count;
    }
    EXPECT_EQ(count, 2);
}

namespace {
    struct alignas(16) TestEvent : babelwires::StreamEvent {
        STREAM_EVENT(TestEvent);

        std::uint64_t m_big[5] = {};
    };
} // namespace

TEST(BlockStream, BlocksAndAlignment) {
    static_assert(sizeof(babelwires::StreamEvent) != sizeof(TestEvent));
    static_assert(alignof(babelwires::StreamEvent) != alignof(TestEvent));

    const size_t loopCount =
        (babelwires::BlockStream::c_blockSize / (sizeof(babelwires::StreamEvent) + sizeof(TestEvent))) * 3;

    babelwires::BlockStream stream;

    for (int i = 0; i < loopCount; ++i) {
        stream.addEvent(babelwires::StreamEvent());
        stream.addEvent(TestEvent()).m_big[2] = i;
    }
    EXPECT_TRUE(stream.getNumEvents() == loopCount * 2);

    int count = 0;
    for (const auto& event : stream) {
        if (count % 2) {
            EXPECT_NE(event.asA<TestEvent>(), nullptr);
            const TestEvent& e = static_cast<const TestEvent&>(event);
            EXPECT_EQ(e.m_big[2], (count / 2));
        }
        ++count;
    }
    EXPECT_EQ(count, loopCount * 2);
}

namespace {
    struct Payload {
        Payload(int& payloadDestructionCounter)
            : m_payloadDestructionCounter(payloadDestructionCounter) {}

        ~Payload() { ++m_payloadDestructionCounter; }

        int& m_payloadDestructionCounter;
    };

    struct EventWithPayload : babelwires::StreamEvent {
        STREAM_EVENT(EventWithPayload);

        EventWithPayload(int& payloadDestructionCounter)
            : m_payload(std::make_unique<Payload>(payloadDestructionCounter)) {}

        EventWithPayload(EventWithPayload&& other)
            : StreamEvent(other) {
            m_payload = std::move(other.m_payload);
        }

        EventWithPayload(const EventWithPayload& other)
            : babelwires::StreamEvent(other)
            , m_payload(std::make_unique<Payload>(*other.m_payload)) {}

        ~EventWithPayload() { m_payload.reset(); }

        std::unique_ptr<Payload> m_payload;
    };
} // namespace

TEST(BlockStream, PayloadTest) {
    const size_t loopCount = (babelwires::BlockStream::c_blockSize / sizeof(EventWithPayload)) * 3;

    // We should observe the payloads being destroyed exactly when the track is destroyed.
    int payloadDestructionCounter = 0;
    {
        babelwires::BlockStream stream;
        for (int i = 0; i < loopCount; ++i) {
            stream.addEvent(EventWithPayload(payloadDestructionCounter));
        }
        EXPECT_EQ(payloadDestructionCounter, 0);
    }
    EXPECT_EQ(payloadDestructionCounter, loopCount);
}

TEST(BlockStream, CopyTest) {
    const size_t loopCount = (babelwires::BlockStream::c_blockSize /
                              (sizeof(babelwires::StreamEvent) + sizeof(EventWithPayload) + sizeof(TestEvent))) *
                             3;

    // This will be shared between the copies, which would not be desirable in real scenarios.
    // However, it let's us ensure the lifetime of payloads is correctly handled.
    int payloadDestructionCounter = 0;
    {
        babelwires::BlockStream stream;

        for (int i = 0; i < loopCount; ++i) {
            stream.addEvent(babelwires::StreamEvent());
            stream.addEvent(EventWithPayload(payloadDestructionCounter));
            stream.addEvent(TestEvent()).m_big[2] = i;
        }

        EXPECT_TRUE(stream.getNumEvents() == loopCount * 3);

        {
            babelwires::BlockStream stream2 = stream;

            EXPECT_TRUE(stream2.getNumEvents() == loopCount * 3);

            int count = 0;
            for (const auto& event : stream2) {
                if (count % 3 == 2) {
                    EXPECT_NE(event.asA<TestEvent>(), nullptr);
                    const TestEvent& e = static_cast<const TestEvent&>(event);
                    EXPECT_EQ(e.m_big[2], (count / 3));
                }
                ++count;
            }
            EXPECT_EQ(count, loopCount * 3);
        }
        EXPECT_EQ(payloadDestructionCounter, loopCount);
    }
    EXPECT_EQ(payloadDestructionCounter, loopCount * 2);
}

TEST(BlockStream, MoveTest) {
    const size_t loopCount = (babelwires::BlockStream::c_blockSize /
                              (sizeof(babelwires::StreamEvent) + sizeof(EventWithPayload) + sizeof(TestEvent))) *
                             3;

    // This will be shared between the copies, which would not be desirable in real scenarios.
    // However, it let's us ensure the lifetime of payloads is correctly handled.
    int payloadDestructionCounter = 0;
    {
        babelwires::BlockStream stream;

        for (int i = 0; i < loopCount; ++i) {
            stream.addEvent(babelwires::StreamEvent());
            stream.addEvent(EventWithPayload(payloadDestructionCounter));
            stream.addEvent(TestEvent()).m_big[2] = i;
        }

        EXPECT_TRUE(stream.getNumEvents() == loopCount * 3);

        {
            babelwires::BlockStream stream2 = std::move(stream);
            EXPECT_EQ(payloadDestructionCounter, 0);

            EXPECT_TRUE(stream2.getNumEvents() == loopCount * 3);

            int count = 0;
            for (const auto& event : stream2) {
                if (count % 3 == 2) {
                    EXPECT_NE(event.asA<TestEvent>(), nullptr);
                    const TestEvent& e = static_cast<const TestEvent&>(event);
                    EXPECT_EQ(e.m_big[2], (count / 3));
                }
                ++count;
            }
            EXPECT_EQ(count, loopCount * 3);
        }
        EXPECT_EQ(payloadDestructionCounter, loopCount);
    }
    EXPECT_EQ(payloadDestructionCounter, loopCount);
}

TEST(BlockStream, CopyAssignTest) {
    const size_t loopCount = (babelwires::BlockStream::c_blockSize /
                              (sizeof(babelwires::StreamEvent) + sizeof(EventWithPayload) + sizeof(TestEvent))) *
                             3;

    // This will be shared between the copies, which would not be desirable in real scenarios.
    // However, it let's us ensure the lifetime of payloads is correctly handled.
    int payloadDestructionCounter = 0;
    {
        babelwires::BlockStream stream;

        for (int i = 0; i < loopCount; ++i) {
            stream.addEvent(babelwires::StreamEvent());
            stream.addEvent(EventWithPayload(payloadDestructionCounter));
            stream.addEvent(TestEvent()).m_big[2] = i;
        }

        EXPECT_TRUE(stream.getNumEvents() == loopCount * 3);

        {
            babelwires::BlockStream stream2;

            for (int i = 0; i < (loopCount - 10); ++i) {
                stream2.addEvent(babelwires::StreamEvent());
                stream2.addEvent(EventWithPayload(payloadDestructionCounter));
                stream2.addEvent(TestEvent()).m_big[2] = i;
            }

            EXPECT_TRUE(stream2.getNumEvents() == (3 * loopCount) - 30);

            stream2 = stream;
            EXPECT_EQ(payloadDestructionCounter, loopCount - 10);

            EXPECT_TRUE(stream2.getNumEvents() == 3 * loopCount);

            int count = 0;
            for (const auto& event : stream2) {
                if (count % 3 == 2) {
                    EXPECT_NE(event.asA<TestEvent>(), nullptr);
                    const TestEvent& e = static_cast<const TestEvent&>(event);
                    EXPECT_EQ(e.m_big[2], (count / 3));
                }
                ++count;
            }
            EXPECT_EQ(count, loopCount * 3);
        }
        EXPECT_EQ(payloadDestructionCounter, (2 * loopCount) - 10);
    }
    EXPECT_EQ(payloadDestructionCounter, (3 * loopCount) - 10);
}

TEST(BlockStream, MoveAssignTest) {
    const size_t loopCount = (babelwires::BlockStream::c_blockSize /
                              (sizeof(babelwires::StreamEvent) + sizeof(EventWithPayload) + sizeof(TestEvent))) *
                             3;

    // This will be shared between the copies, which would not be desirable in real scenarios.
    // However, it let's us ensure the lifetime of payloads is correctly handled.
    int payloadDestructionCounter = 0;
    {
        babelwires::BlockStream stream;

        for (int i = 0; i < loopCount; ++i) {
            stream.addEvent(babelwires::StreamEvent());
            stream.addEvent(EventWithPayload(payloadDestructionCounter));
            stream.addEvent(TestEvent()).m_big[2] = i;
        }

        EXPECT_TRUE(stream.getNumEvents() == loopCount * 3);

        {
            babelwires::BlockStream stream2;

            for (int i = 0; i < loopCount - 10; ++i) {
                stream2.addEvent(babelwires::StreamEvent());
                stream2.addEvent(EventWithPayload(payloadDestructionCounter));
                stream2.addEvent(TestEvent()).m_big[2] = i;
            }

            EXPECT_TRUE(stream2.getNumEvents() == (3 * loopCount) - 30);

            stream2 = std::move(stream);
            EXPECT_EQ(payloadDestructionCounter, loopCount - 10);

            EXPECT_TRUE(stream2.getNumEvents() == 3 * loopCount);

            int count = 0;
            for (const auto& event : stream2) {
                if (count % 3 == 2) {
                    EXPECT_NE(event.asA<TestEvent>(), nullptr);
                    const TestEvent& e = static_cast<const TestEvent&>(event);
                    EXPECT_EQ(e.m_big[2], (count / 3));
                }
                ++count;
            }
            EXPECT_EQ(count, (3 * loopCount));
        }
        EXPECT_EQ(payloadDestructionCounter, (2 * loopCount) - 10);
    }
    EXPECT_EQ(payloadDestructionCounter, (2 * loopCount) - 10);
}
