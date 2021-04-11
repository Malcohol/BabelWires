#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace babelwires {

    /// A very simple UUID class which is essentially just a wrapper for a validated string.
    /// This does not impose any standard on the UUIDs, it just requires that they have the
    /// the required textual format.
    class Uuid {
      public:
        /// This asserts the uuidText is in the correct form:
        /// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        Uuid(std::string uuidText);

        /// This asserts the uuidText is in the correct form:
        /// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        Uuid(const char* uuidText)
            : Uuid(std::string(uuidText)) {}

        std::string serializeToString() const { return m_text; }

        /// This throws a parse exception if the uuidText is not in the correct form:
        /// xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
        static Uuid deserializeFromString(std::string_view uuidText);

        /// Write the UUID in textual form to the stream.
        friend std::ostream& operator<<(std::ostream& os, const Uuid& uuid) { return os << uuid.m_text; }

        friend bool operator==(const Uuid& a, const Uuid& b) { return a.m_text == b.m_text; }

        friend bool operator!=(const Uuid& a, const Uuid& b) { return !(a == b); }

      private:
        /// This just confirms the arrangement of digits and hyphens is correct, and
        /// does not impose any standard.
        static bool isValid(std::string_view uuidText);

        friend struct std::hash<Uuid>;

      private:
        /// For simplicity, just store as text.
        std::string m_text;
    };

} // namespace babelwires

namespace std {
    /// Calculate a hash for a UUID.
    template <> struct hash<babelwires::Uuid> {
        inline std::size_t operator()(const babelwires::Uuid& uuid) const { return m_stringHasher(uuid.m_text); }

        std::hash<std::string> m_stringHasher;
    };
} // namespace std
