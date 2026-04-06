/**
 * Thread-safe randomness service with a canonical root seed.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#include <BaseLib/Hash/hash.hpp>
#include <BaseLib/Random/randomService.hpp>

#include <unordered_map>

std::atomic<std::uint64_t> babelwires::RandomService::s_nextRandomServiceId{1};

babelwires::RandomService::RandomService(const std::uint64_t rootSeed)
    : m_randomServiceId(getIdForNewRandomService())
    , m_rootSeed(rootSeed)
    , m_nextSequenceNumber(0) {}

babelwires::RandomService::RandomEngine& babelwires::RandomService::getEngineForThisThread() const {
    thread_local std::unordered_map<std::uint64_t, RandomEngine> threadEngines;

    auto [it, inserted] = threadEngines.try_emplace(m_randomServiceId);
    if (inserted) {
        it->second.seed(getSeedForNewRandomEngine());
    }

    return it->second;
}

std::uint64_t babelwires::RandomService::getIdForNewRandomService() {
    return s_nextRandomServiceId.fetch_add(1, std::memory_order_relaxed);
}

std::uint64_t babelwires::RandomService::getSeedForNewRandomEngine() const {
    constexpr std::uint64_t c_goldenRatio64 = 0x9e3779b97f4a7c15ull;
    const std::uint64_t nextSequenceNumber = m_nextSequenceNumber.fetch_add(1, std::memory_order_relaxed);
    return static_cast<std::uint64_t>(babelwires::hash::mixtureOf(m_rootSeed, nextSequenceNumber, c_goldenRatio64));
}
