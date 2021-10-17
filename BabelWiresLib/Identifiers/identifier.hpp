/**
 * An identifier uniquely identifies an object in some local context (e.g. a field in a record) and globally.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>
#include <string>
#include <string_view>

namespace babelwires {

    constexpr char s_pathDelimiterString[] = "/";
    constexpr char s_pathDelimiter = s_pathDelimiterString[0];
    constexpr char s_discriminatorDelimiter = '`';

    /// An identifier uniquely identifies an object in some local context and globally.
    /// The primary use-case is identifying fields within records.
    ///
    /// It stores a short string (6 bytes) and a numeric discriminator (2 bytes).
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
    /// Identifiers have the following benefits:
    /// * They are small
    /// * They are cheap to compare
    /// * They provide a canonical form for fields when serializing and de-serializing paths.
    /// * They sort in alphabetic order (deterministic and very useful for debugging)
    /// * They can be examined in any debugger without a visualizer.
    ///
    /// Constraints on characters:
    /// * They must have a graphical representation (see std::isgraph)
    /// * They cannot be the path delimiter "/"
    /// * They cannot be the discriminator delimiter "`"
    ///
    union Identifier {
      public:
        /// The maximum size string which can be contained.
        static constexpr unsigned int N = 6;

        /// Constructor from a string literal.
        template <unsigned int M, typename std::enable_if_t<(2 < M) && (M <= N), int> = 0>
        Identifier(const char (&str)[M]) {
            assert(validate(str, M - 1) && "The identifier is invalid");
            // TODO Big endian.
            // std::copy(str, str + M - 1, m_data.m_chars);
            // std::fill(m_data.m_chars + M - 1, m_data.m_chars + N + 1, 0);
            std::reverse_copy(str, str + M - 1, m_data.m_chars + N - M + 1);
            std::fill(m_data.m_chars, m_data.m_chars + N - M + 1, 0);
        }

        Identifier(const char (&str)[N + 1]) {
            assert(validate(str, N) && "The identifier is invalid");
            // TODO Big endian
            // std::copy(str, str + N + 1, m_data.m_chars);
            std::reverse_copy(str, str + N, m_data.m_chars);
        }

        /// Statically exclude literals which are too short or too long.
        template <unsigned int M, typename std::enable_if_t<(1 == M) || (M > N + 1), int> = 0>
        Identifier(const char (&str)[M]) = delete;

        /// Construct an identifier from a non-static string.
        Identifier(std::string_view str);

        /// Return a serializable version of the identifier.
        std::string serializeToString() const;

        /// Parse a string as an identifier. This will parse disciminators too.
        /// This throws a ParseException if the identifier is not valid.
        static Identifier deserializeFromString(std::string_view str);

        friend bool operator==(const Identifier& a, const Identifier& b) {
            return a.getDataAsCode() == b.getDataAsCode();
        }

        friend bool operator!=(const Identifier& a, const Identifier& b) {
            return a.getDataAsCode() != b.getDataAsCode();
        }

        friend bool operator<(const Identifier& a, const Identifier& b) {
            return a.getDataAsCode() < b.getDataAsCode();
        }

        friend bool operator<=(const Identifier& a, const Identifier& b) {
            return a.getDataAsCode() <= b.getDataAsCode();
        }

        friend std::ostream& operator<<(std::ostream& os, const Identifier& identifier) {
            identifier.writeToStream(os);
            return os;
        }

      public:
        /// The numeric type which distinguishes two identifiers with the same textual content.
        using Discriminator = std::uint16_t;

        /// We need to reserve at least 1 so we can distinguish ArrayIndexes from identifiers when parsing paths.
        /// Reserve several just in case.
        static constexpr Discriminator c_maxDiscriminator = std::numeric_limits<Identifier::Discriminator>::max() - 10;

        /// Get the discriminator (which distinguishes between identifiers with the same textual content).
        Discriminator getDiscriminator() const { return m_data.m_discriminator; }

        /// Set the discriminator (which distinguishes between identifiers with the same textual content).
        void setDiscriminator(Discriminator index) const { m_data.m_discriminator = index; }

        /// Get an integer corresponding to the texture part of the identifier, and not the discriminator.
        std::uint64_t getDataAsCode() const { return m_code & 0xffffffffffff0000; }

        /// If other doesn't have a discriminator set, set its discriminator to the discrimintor of this.
        void copyDiscriminatorTo(const Identifier& other) const {
            if (other.getDiscriminator() == 0) {
                copyDiscriminatorToInternal(other);
            }
        }

      private:
        /// Called by the constructors to ensure the data is valid.
        static bool validate(const char* chars, size_t n);

        void copyDiscriminatorToInternal(const Identifier& other) const;

        void writeToStream(std::ostream& os) const;

      private:
        friend struct std::hash<babelwires::Identifier>;

      private:
        // A union of these.

        // TODO Big-endian.
        struct Data {
            mutable std::uint16_t m_discriminator;
            char m_chars[N];
        } m_data = {0};

        /// An integer equivalent to the contents of the identifier.
        /// Used for efficient comparison.
        std::uint64_t m_code;

        static_assert(sizeof(Data) == sizeof(m_code));
    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::Identifier> {
        std::size_t operator()(const babelwires::Identifier& identifier) const {
            return m_hasher(identifier.getDataAsCode());
        }

        std::hash<std::uint64_t> m_hasher;
    };
} // namespace std
