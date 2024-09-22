/**
 * A PathStep is a union of a ShortId and an ArrayIndex.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Identifiers/identifier.hpp>

#include <ostream>

namespace babelwires {

    using ArrayIndex = std::uint16_t;

    class IdentifierRegistry;

    /// A PathStep is a union of a ShortId and an ArrayIndex.
    union PathStep {
      public:
        /// A default constructed step is neither a field nor an index.
        /// Not-step steps are not permitted in paths, so most code can ignore this type of step
        /// (or assert that it is not expected).
        /// This is just a convenient way of representing an optional step.
        PathStep() : m_notAStep() {}

        // explicit, because a temporary PathStep contains a copy of f, and any modification to its mutable
        // discriminator will be lost.
        // TODO This was a mistake. Allow implicit construction from a field, and approach discriminator resolution
        // a different way.
        explicit PathStep(const ShortId& f)
            : m_fieldIdentifier(f) {}

        PathStep(ArrayIndex index)
            : m_arrayIndex(Index()) {
            m_arrayIndex.m_index = index;
            // Leave some spare values to detect issues, such as underflow.
            assert((index < 65000) && "Index too large");
        }

        /// Identifier Discriminators are not permitted to reach this high.
        static constexpr ArrayIndex paddingVal = 0xffff;

        /// Does this step contain a field?
        bool isField() const { return m_arrayIndex.m_padding[0] != paddingVal; }

        /// Does this step contain an array index?
        bool isIndex() const { return m_arrayIndex.m_padding[1] == paddingVal; }

        /// Does this step actually not represent a step at all.
        bool isNotAStep() const { return m_arrayIndex.m_padding[2] == paddingVal; }

        /// Get the contained field identifier or assert.
        const ShortId& getField() const {
            assert(isField());
            return m_fieldIdentifier;
        }

        /// Get the contained field identifier or assert.
        ShortId& getField() {
            assert(isField());
            return m_fieldIdentifier;
        }

        /// Get the contained index or assert.
        ArrayIndex getIndex() const {
            assert(isIndex());
            return m_arrayIndex.m_index;
        }

        /// Get the contained index or assert.
        ArrayIndex& getIndex() {
            assert(isIndex());
            return m_arrayIndex.m_index;
        }

        /// If the step contains a field identifier, return a pointer to it.
        const ShortId* asField() const { return isField() ? &m_fieldIdentifier : nullptr; }

        /// If the step contains an array index, return a pointer to it.
        const ArrayIndex* asIndex() const { return isIndex() ? &m_arrayIndex.m_index : nullptr; }

        std::string serializeToString() const;

        /// Parse a string as a path step.
        /// This can throw a ParseException.
        static PathStep deserializeFromString(std::string_view str);

        friend bool operator==(const PathStep& a, const PathStep& b) { return a.getDataAsCode() == b.getDataAsCode(); }
        friend bool operator!=(const PathStep& a, const PathStep& b) { return a.getDataAsCode() != b.getDataAsCode(); }
        friend bool operator<(const PathStep& a, const PathStep& b) { return a.getDataAsCode() < b.getDataAsCode(); }
        friend bool operator<=(const PathStep& a, const PathStep& b) { return a.getDataAsCode() <= b.getDataAsCode(); }

        friend std::ostream& operator<<(std::ostream& os, const PathStep& step) {
            step.writeToStream(os);
            return os;
        }

        /// Write to the stream in a human-readable way.
        void writeToStreamReadable(std::ostream& os, const IdentifierRegistry& identifierRegistry) const;

        /// Assuming this and other are equal fields, copy the discriminator from other to this.
        /// TODO: Avoid needing this to be const and relying on discriminators being mutable.
        void copyDiscriminatorFrom(const PathStep& other) const {
            if (const ShortId* thisField = asField()) {
                const ShortId* otherField = other.asField();
                assert(otherField);
                assert(*thisField == *otherField);
                otherField->copyDiscriminatorTo(*thisField);
            }
        }

        /// How the not-a-step value is represented as a string.
        static constexpr char c_notAStepRepresentation[] = "(notAStep)";

      private:
        /// Get a efficient representation of the contents of this object.
        std::uint64_t getDataAsCode() const { return m_code & 0xffffffffffff0000; }

        void writeToStream(std::ostream& os) const;

      private:
        friend struct std::hash<PathStep>;

        // A union of these.
        ShortId m_fieldIdentifier;

        struct Index {
            /// The last two bytes are used as the tag.
            std::uint16_t m_padding[3] = {paddingVal, paddingVal, 0};
            ArrayIndex m_index;
        } m_arrayIndex;

        struct NotAStep {
            std::uint16_t m_notAStep[4] = { paddingVal, 0, paddingVal, 0 };
        } m_notAStep;

        /// Used for efficient comparison.
        std::uint64_t m_code;

        static_assert(sizeof(m_fieldIdentifier) == sizeof(m_arrayIndex));
        static_assert(sizeof(m_fieldIdentifier) == sizeof(m_code));
    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::PathStep> {
        inline std::size_t operator()(const babelwires::PathStep& step) const { return m_hasher(step.getDataAsCode()); }
        std::hash<std::uint64_t> m_hasher;
    };
} // namespace std
