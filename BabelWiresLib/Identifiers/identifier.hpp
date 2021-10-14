/**
 * A short string which uniquely defines a field in a record, with a discriminator which
 * makes it unique in the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <cassert>
#include <cstring>
#include <functional>
#include <string>
#include <string_view>
#include <algorithm>

namespace babelwires {

    constexpr char s_pathDelimiterString[] = "/";
    constexpr char s_pathDelimiter = s_pathDelimiterString[0];
    constexpr char s_discriminatorDelimiter = '`';

    /// A short string which uniquely defines a field in a record, with a discriminator which
    /// makes it unique in the project.
    /// Stores a string of length 7 in-place, and 1 reserved byte.
    /// The string is stored in reverse order, and may not be null-terminated.
    /// The intention is to have efficient comparison and reasonable
    /// debuggability.
    ///
    /// Constraints:
    /// * characters must have a graphical representation (see std::isgraph).
    /// * it may not contain the path delimiter
    /// * it may not contain the discriminator delimiter
    ///
    // TODO code for big endian architectures.
    union FieldIdentifier {
      public:
        /// The maximum size string which can be contained.
        static constexpr unsigned int N = 6;

        /// Constructor from a string literal.
        template <unsigned int M, typename std::enable_if_t<(2 < M) && (M <= N), int> = 0>
        FieldIdentifier(const char (&str)[M]) {
            assert(validate(str, M - 1) && "The identifier is invalid");
            // TODO Big endian.
            // std::copy(str, str + M - 1, m_data.m_chars);
            // std::fill(m_data.m_chars + M - 1, m_data.m_chars + N + 1, 0);
            std::reverse_copy(str, str + M - 1, m_data.m_chars + N - M + 1);
            std::fill(m_data.m_chars, m_data.m_chars + N - M + 1, 0);
        }

        FieldIdentifier(const char (&str)[N + 1]) {
            assert(validate(str, N) && "The identifier is invalid");
            // TODO Big endian
            // std::copy(str, str + N + 1, m_data.m_chars);
            std::reverse_copy(str, str + N, m_data.m_chars);
        }

        /// Statically exclude literals which are too short or too long.
        template <unsigned int M, typename std::enable_if_t<(1 == M) || (M > N + 1), int> = 0>
        FieldIdentifier(const char (&str)[M]) = delete;

        /// Construct an identifier from a non-static string.
        FieldIdentifier(std::string_view str);

        /// Return a serializable version of the identifier.
        std::string serializeToString() const;

        /// Parse a string as a field identifier. This will parse disciminators too.
        /// This throws a ParseException if the identifier is not valid.
        static FieldIdentifier deserializeFromString(std::string_view str);

        friend bool operator==(const FieldIdentifier& a, const FieldIdentifier& b) {
            return a.getDataAsCode() == b.getDataAsCode();
        }

        friend bool operator!=(const FieldIdentifier& a, const FieldIdentifier& b) {
            return a.getDataAsCode() != b.getDataAsCode();
        }

        friend bool operator<(const FieldIdentifier& a, const FieldIdentifier& b) {
            return a.getDataAsCode() < b.getDataAsCode();
        }

        friend bool operator<=(const FieldIdentifier& a, const FieldIdentifier& b) {
            return a.getDataAsCode() <= b.getDataAsCode();
        }

        friend std::ostream& operator<<(std::ostream& os, const FieldIdentifier& identifier) {
            identifier.writeToStream(os);
            return os;
        }

      public:
        /// The numeric type which distinguishes two field identifiers with the same textual content.
        using Discriminator = std::uint16_t;

        /// We need to reserve at least 1 so we can distinguish ArrayIndexes from fields. Reserve several just in case.
        static constexpr Discriminator c_maxDiscriminator = std::numeric_limits<FieldIdentifier::Discriminator>::max() - 10; 

        /// Allows two field identifiers to represent two different fields.
        Discriminator getDiscriminator() const { return m_data.m_discriminator; }

        /// Allows two field identifiers to represent two different fields.
        void setDiscriminator(Discriminator index) const { m_data.m_discriminator = index; }

        std::uint64_t getDataAsCode() const { return m_code & 0xffffffffffff0000; }

        /// If other doesn't have a discriminator set, set its discriminator to the discrimintor of this.
        void copyDiscriminatorTo(const FieldIdentifier& other) const {
            if (other.getDiscriminator() == 0) {
                copyDiscriminatorToInternal(other);
            }
        }

      private:
        /// Called by the constructors to ensure the data is valid.
        static bool validate(const char* chars, size_t n);

        void copyDiscriminatorToInternal(const FieldIdentifier& other) const;

        void writeToStream(std::ostream& os) const;

      private:
        friend struct std::hash<babelwires::FieldIdentifier>;

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
    template <> struct hash<babelwires::FieldIdentifier> {
        std::size_t operator()(const babelwires::FieldIdentifier& identifier) const {
            return m_hasher(identifier.getDataAsCode());
        }

        std::hash<std::uint64_t> m_hasher;
    };
} // namespace std
