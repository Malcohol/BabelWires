#include <BaseLib/Random/randomService.hpp>

#include <gtest/gtest.h>

#include <barrier>
#include <cstdint>
#include <set>
#include <thread>
#include <vector>

namespace {

    std::vector<std::uint64_t> collectValuesFromSingleThreadStream(const std::uint64_t rootSeed, const int count) {
        babelwires::RandomService randomService(rootSeed);
        auto& engine = randomService.getRandomEngine();
        std::uniform_int_distribution<std::uint64_t> distribution;

        std::vector<std::uint64_t> values;
        values.reserve(count);
        for (int i = 0; i < count; ++i) {
            values.push_back(distribution(engine));
        }
        return values;
    }

    std::vector<std::uint64_t> collectFirstValuesFromNewThreadStreams(const std::uint64_t rootSeed,
                                                                      const int streamCount) {
        babelwires::RandomService randomService(rootSeed);
        std::vector<std::uint64_t> values;
        values.reserve(streamCount);

        for (int i = 0; i < streamCount; ++i) {
            std::uint64_t value = 0;
            std::thread worker([&]() {
                auto& engine = randomService.getRandomEngine();
                std::uniform_int_distribution<std::uint64_t> distribution;
                value = distribution(engine);
            });
            worker.join();
            values.push_back(value);
        }

        return values;
    }

} // namespace

TEST(RandomServiceTest, streamsAreDerivedDeterministicallyFromRootSeed) {
    constexpr std::uint64_t seedA = 0x123456789abcdef0ull;
    constexpr std::uint64_t seedB = 0x0fedcba987654321ull;

    const std::vector<std::uint64_t> singleThreadA1 = collectValuesFromSingleThreadStream(seedA, 16);
    const std::vector<std::uint64_t> singleThreadA2 = collectValuesFromSingleThreadStream(seedA, 16);
    const std::vector<std::uint64_t> singleThreadB = collectValuesFromSingleThreadStream(seedB, 16);

    EXPECT_EQ(singleThreadA1, singleThreadA2);
    EXPECT_NE(singleThreadA1, singleThreadB);

    const std::vector<std::uint64_t> threadStreamsA1 = collectFirstValuesFromNewThreadStreams(seedA, 8);
    const std::vector<std::uint64_t> threadStreamsA2 = collectFirstValuesFromNewThreadStreams(seedA, 8);
    const std::vector<std::uint64_t> threadStreamsB = collectFirstValuesFromNewThreadStreams(seedB, 8);

    EXPECT_EQ(threadStreamsA1, threadStreamsA2);
    EXPECT_NE(threadStreamsA1, threadStreamsB);
}

TEST(RandomServiceTest, providesDistinctThreadLocalEnginesForConcurrentThreads) {
    constexpr std::uint64_t seed = 0x2468ace013579bdfull;
    constexpr int threadCount = 8;

    babelwires::RandomService randomService(seed);
    std::vector<std::uintptr_t> engineAddresses(threadCount);
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    std::barrier syncPoint(threadCount);
    std::barrier addressesCaptured(threadCount);

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&, i]() {
            syncPoint.arrive_and_wait();
            auto& engine = randomService.getRandomEngine();
            engineAddresses[i] = reinterpret_cast<std::uintptr_t>(&engine);
            std::uniform_int_distribution<std::uint64_t> distribution;
            (void)distribution(engine);
            // Ensure all threads have captured their engine address before any thread
            // exits and destroys its thread-local engine, which could allow address reuse.
            addressesCaptured.arrive_and_wait();
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    const std::set<std::uintptr_t> uniqueAddresses(engineAddresses.begin(), engineAddresses.end());
    EXPECT_EQ(static_cast<int>(uniqueAddresses.size()), threadCount);

    auto* const mainThreadEngineA = &randomService.getRandomEngine();
    auto* const mainThreadEngineB = &randomService.getRandomEngine();
    EXPECT_EQ(mainThreadEngineA, mainThreadEngineB);
}
