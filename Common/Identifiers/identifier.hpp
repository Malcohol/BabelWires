/**
 * An identifier uniquely identifies an object in some local context (e.g. a field in a record) and globally.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Utilities/hash.hpp>
#include <Common/exceptions.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>
#include <limits>
#include <string>
#include <string_view>
#include <array>

namespace babelwires {

    constexpr char s_pathDelimiterString[] = "/";
    constexpr char s_pathDelimiter = s_pathDelimiterString[0];
    constexpr char s_discriminatorDelimiter = '`';

    /// An identifier uniquely identifies an object in some local context and globally.
    /// The primary use-case is identifying fields within records.
    ///
    /// It stores a short string (N bytes) and a numeric discriminator (2 bytes).
    /// The string should be sufficient on its own to uniquely define an object during
    /// local usage (e.g. a field within a record, or an Enum value within an Enum).
    /// The discriminator is used to ensure the identifier is globally unique, which allows
    /// the identifier to act as a key for look-up in a global registry.
    ///
    /// Global uniqueness is achieved by registering the identifier with the IdentifierRegistry.
    /// The most convenient way of registering an identifier is to use the REGISTERED_ID macro.
    ///
    /// Data is arranged so the bit pattern can never be confused for array indices, when
    /// considering FeaturePaths. On little-endian architectures, the string is stored in
    /// reverse order. (On big-endian architectures, the string would be stored in the
    /// normal way, but that's a todo.)
    ///
    /// Identifiers have the following benefits (when NUM_BLOCKS == 1):
    /// * They are small
    /// * They are cheap to compare
    /// * They provide a canonical and somewhat readable representation for object identity when
    ///   serializing.
    /// * They sort in alphabetic order (deterministic and very useful for debugging)
    /// * They can be examined in any debugger without a visualizer.
    /// * The registry will store an identifier's metadata (Uuid & name) even if the identifier
    ///   is deserialized into a build where the referant does not exist. Thus, the metadata will
    ///   be available if the identifier is later serialized.
    ///
    /// Constraints on characters:
    /// * They must have a graphical representation (see std::isgraph)
    /// * They cannot be the path delimiter "/"
    /// * They cannot be the discriminator delimiter "`"
    ///
    /// NUM_BLOCKS is the number of std::uint64_t which store the contents.
    template <unsigned int NUM_BLOCKS> union IdentifierBase {
      public:
        /// The maximum number of characters which can be contained.
        static constexpr unsigned int N = (NUM_BLOCKS * sizeof(std::uint64_t)) - 2;

        IdentifierBase() : IdentifierBase("_unset") {}

        /// Constructor from a string literal.
        template <unsigned int M, typename std::enable_if_t<(1 < M) && (M <= N), int> = 0>
        IdentifierBase(const char (&str)[M]) {
            assert(validate(str, M - 1) && "The identifier is invalid");
            // TODO Big endian.
            // std::copy(str, str + M - 1, m_data.m_chars);
            // std::fill(m_data.m_chars + M - 1, m_data.m_chars + N + 1, 0);
            std::reverse_copy(str, str + M - 1, m_data.m_chars + N - M + 1);
            std::fill(m_data.m_chars, m_data.m_chars + N - M + 1, 0);
        }

        IdentifierBase(const char (&str)[N + 1]) {
            assert(validate(str, N) && "The identifier is invalid");
            // TODO Big endian
            // std::copy(str, str + N + 1, m_data.m_chars);
            std::reverse_copy(str, str + N, m_data.m_chars);
        }

        /// Statically exclude literals which are too long.
        template <unsigned int M, typename std::enable_if_t<(M > N + 1), int> = 0>
        IdentifierBase(const char (&str)[M]) = delete;

        /// Construct an identifier from a non-static string.
        IdentifierBase(std::string_view str);

        /// Short to long constructor. Always succeeds.
        template <unsigned int OTHER_NUM_BLOCKS, typename std::enable_if_t<(OTHER_NUM_BLOCKS <= NUM_BLOCKS), int> = 0>
        IdentifierBase(const IdentifierBase<OTHER_NUM_BLOCKS>& other) {
            constexpr unsigned int M = (OTHER_NUM_BLOCKS * sizeof(std::uint64_t)) - 2;
            constexpr unsigned int D = N - M;
            m_data.m_discriminator = other.m_data.m_discriminator;
            std::copy(other.m_data.m_chars, other.m_data.m_chars + M, m_data.m_chars + D);
            std::fill(m_data.m_chars, m_data.m_chars + D, 0);
        }

        /// Long to short constructor. Can throw a parse exception if the contents are too long.
        template <unsigned int OTHER_NUM_BLOCKS, typename std::enable_if_t<(OTHER_NUM_BLOCKS > NUM_BLOCKS), int> = 0>
        explicit IdentifierBase(const IdentifierBase<OTHER_NUM_BLOCKS>& other) {
            constexpr unsigned int M = (OTHER_NUM_BLOCKS * sizeof(std::uint64_t)) - 2;
            constexpr unsigned int D = M - N;
            if (other.m_data.m_chars[D - 1] != '\0') {
                throw ParseException() << "The contents of identifier '" << other << "' are too long";
            }
            m_data.m_discriminator = other.m_data.m_discriminator;
            std::copy(other.m_data.m_chars + D, other.m_data.m_chars + M, m_data.m_chars);
        }

        /// Return a human-readable version of the identifier, not including the disciminator.
        std::string toString() const;

        /// Return a serializable version of the identifier.
        std::string serializeToString() const;

        /// Parse a string as an identifier. This will parse disciminators too.
        /// This throws a ParseException if the identifier is not valid.
        static IdentifierBase deserializeFromString(std::string_view str);

        /// Shorter identifiers can be converted to a code, which does excludes the discriminator.
        template <int T = NUM_BLOCKS, typename std::enable_if_t<(T == 1), std::nullptr_t> = nullptr>
        std::uint64_t toCode() const { return getDataAsCode<0>(); }

      public:
        friend bool operator==(const IdentifierBase& a, const IdentifierBase& b) {
            return a.getTupleOfCodes() == b.getTupleOfCodes();
        }

        friend bool operator!=(const IdentifierBase& a, const IdentifierBase& b) {
            return a.getTupleOfCodes() != b.getTupleOfCodes();
        }

        friend bool operator<(const IdentifierBase& a, const IdentifierBase& b) {
            return a.getTupleOfCodes() < b.getTupleOfCodes();
        }

        friend bool operator<=(const IdentifierBase& a, const IdentifierBase& b) {
            return a.getTupleOfCodes() <= b.getTupleOfCodes();
        }

        friend std::ostream& operator<<(std::ostream& os, const IdentifierBase& identifier) {
            identifier.writeTextToStream(os);
            return os;
        }

      public:
        /// The numeric type which distinguishes two identifiers with the same textual content.
        using Discriminator = std::uint16_t;

        /// We need to reserve at least 1 so we can distinguish ArrayIndexes from identifiers when parsing paths.
        /// Reserve several just in case.
        static constexpr Discriminator c_maxDiscriminator =
            std::numeric_limits<IdentifierBase::Discriminator>::max() - 10;

        /// Get the discriminator (which distinguishes between identifiers with the same textual content).
        Discriminator getDiscriminator() const;

        /// Set the discriminator (which distinguishes between identifiers with the same textual content).
        void setDiscriminator(Discriminator index) const;

        /// If other doesn't have a discriminator set, set its discriminator to the discrimintor of this.
        void copyDiscriminatorTo(const IdentifierBase& other) const;

      private:
        /// Called by the constructors to ensure the data is valid.
        static bool validate(const char* chars, size_t n);

        /// Write the textual portion of the identifier to the stream.
        void writeTextToStream(std::ostream& os) const;

        // Comparisons and hash calculations are based on the integer representation, but we need to mask out the
        // discriminator.

        /// Get an integer corresponding to the textual part of the identifier, and not the discriminator.
        // TODO Big-endian indexing would take from the other end.
        template <unsigned int M, typename std::enable_if_t<(M == NUM_BLOCKS - 1), int> = 0>
        std::uint64_t getDataAsCode() const {
            return m_code[0] & 0xffffffffffff0000;
        }

        template <unsigned int M, typename std::enable_if_t<(M < NUM_BLOCKS - 1), int> = 0>
        std::uint64_t getDataAsCode() const {
            return m_code[NUM_BLOCKS - 1 - M];
        }

        /// Helper method which uses the integer sequence to map getDataAsCode over the m_code array.
        template <size_t... INSEQ> auto getTupleOfCodesFromIndexSequence(std::index_sequence<INSEQ...>) const;

        /// Helper method to build a tuple of codes.
        /// The order of codes ensures lexicographic sorting of the texture contents.
        auto getTupleOfCodes() const {
            return getTupleOfCodesFromIndexSequence(std::make_index_sequence<NUM_BLOCKS>{});
        }

        /// Helper method to build a hash out of the codes.
        template <size_t... INSEQ> std::size_t getHashFromIndexSequence(std::index_sequence<INSEQ...>) const;

      private:
        friend struct std::hash<babelwires::IdentifierBase<NUM_BLOCKS>>;
        template <unsigned int OTHER_NUM_BLOCKS> friend union IdentifierBase;

      private:
        // A union of these.

        // TODO Big-endian.
        struct Data {
            mutable std::uint16_t m_discriminator;
            char m_chars[N];
        } m_data = {0};

        /// An integer equivalent to the contents of the identifier.
        /// Used for efficient comparison.
        std::array<std::uint64_t, NUM_BLOCKS> m_code;

        static_assert(sizeof(Data) == sizeof(m_code));
    };

    using Identifier = IdentifierBase<1>;

    using LongIdentifier = IdentifierBase<3>;

} // namespace babelwires

namespace std {
    template <unsigned int NUM_BLOCKS> struct hash<babelwires::IdentifierBase<NUM_BLOCKS>> {
        std::size_t operator()(const babelwires::IdentifierBase<NUM_BLOCKS>& identifier) const {
            return identifier.getHashFromIndexSequence(std::make_index_sequence<NUM_BLOCKS>{});
        }
    };
} // namespace std

#include <Common/Identifiers/identifier_inl.hpp>