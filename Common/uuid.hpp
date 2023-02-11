/**
 * A very simple UUID class.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <ostream>
#include <random>

#include <Common/Hash/hash.hpp>

namespace babelwires {

    /// A very simple UUID class.
    /// This does not impose any standard on the UUIDs, it just requires that they have the
    /// the required textual format.
    class Uuid {
      public:
        /// Initializes the uuid to the zero pattern.
        Uuid();

        /// This asserts the uuidText is in the correct form:
        /// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        Uuid(const std::string& uuidText);

        /// This asserts the uuidText is in the correct form:
        /// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        Uuid(const char* uuidText)
            : Uuid(std::string(uuidText)) {}

        /// Is the Uuid all zeros?
        bool isZero() const;

        /// Set the Uuid to a random, non-zero value.
        void randomize(std::default_random_engine& randomEngine);

        std::string serializeToString() const;

        /// This throws a parse exception if the uuidText is not in the correct form:
        /// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        static Uuid deserializeFromString(std::string_view uuidText);

        /// Write the UUID in textual form to the stream.
        friend std::ostream& operator<<(std::ostream& os, const Uuid& uuid) { return os << uuid.serializeToString(); }

        friend bool operator==(const Uuid& a, const Uuid& b) { return (a.m_high == b.m_high) && (a.m_low == b.m_low); }

        friend bool operator!=(const Uuid& a, const Uuid& b) { return !(a == b); }

      private:
        /// This just confirms the arrangement of digits and hyphens is correct, and
        /// does not impose any standard.
        static bool isValid(std::string_view uuidText);

        friend struct std::hash<Uuid>;

      private:
        std::uint64_t m_high = 0;
        std::uint64_t m_low = 0;
    };

} // namespace babelwires

namespace std {
    /// Calculate a hash for a UUID.
    template <> struct hash<babelwires::Uuid> {
        inline std::size_t operator()(const babelwires::Uuid& uuid) const { return babelwires::hash::mixtureOf(uuid.m_high, uuid.m_low); }
    };
} // namespace std
