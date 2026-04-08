/**
 * Thread-safe randomness service with a canonical root seed.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/baseLibExport.hpp>

#include <atomic>
#include <cstdint>
#include <random>

namespace babelwires {

    /// RandomService derives all randomness from a single canonical root seed.
    ///
    /// Access is thread-safe and efficient: each thread lazily initializes its own
    /// engine stream derived from the canonical seed, avoiding a shared mutex in
    /// hot paths.
    ///
    /// It is not expected that arbitrary multithreaded use of the engines will be deterministic.
    class BASELIB_API RandomService {
      public:
        /// Construct using a provided seed.
        explicit RandomService(std::uint64_t rootSeed);

        using RandomEngine = std::default_random_engine;

        /// Returns a random engine safely usable by the current thread.
        RandomEngine& getRandomEngine() const;

      public:
        RandomService(const RandomService&) = delete;
        RandomService& operator=(const RandomService&) = delete;
        RandomService(RandomService&&) = delete;
        RandomService& operator=(RandomService&&) = delete;

      private:
        /// Get an ID for a new RandomService.
        static std::uint64_t getIdForNewRandomService();

        /// Get a seed for a new random engine, derived from the root seed and the stream number.
        std::uint64_t getSeedForNewRandomEngine() const;

      private:
        /// The service id is used to to ensure different RandomServices do not interfere with each other's thread-local
        /// engines.
        static std::atomic<std::uint64_t> s_nextRandomServiceId;
        const std::uint64_t m_randomServiceId;
        const std::uint64_t m_rootSeed;
        /// The sequence number is used to derive seeds for new random engines, and is incremented atomically to ensure
        /// unique seeds.
        mutable std::atomic<std::uint64_t> m_nextSequenceNumber;
    };

} // namespace babelwires
