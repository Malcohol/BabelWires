/**
 * A Value is an abstract class for objects which carry a single, usually simple value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/Cloning/cloneable.hpp>
#include <Common/Identifiers/identifier.hpp>

namespace babelwires {
    class Type;
    class EditableValue;

    /// A Value is an abstract class for objects which carry a single, usually simple value.
    /// Value lifetimes are usually managed by the ValueHolder container.
    class Value : public Cloneable {
      public:
        DOWNCASTABLE_TYPE_HIERARCHY(Value);
        CLONEABLE_ABSTRACT(Value);

        virtual std::size_t getHash() const = 0;
        virtual bool operator==(const Value& other) const = 0;

        /// Convenience method for checking whether the value is an editable value.
        const EditableValue* asEditableValue() const;

        bool operator!=(const Value& other) const { return !(*this == other); }
    };

    class SimpleValue : Value {
        public:
    };
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::Value> {
        inline std::size_t operator()(const babelwires::Value& value) const { return value.getHash(); }
    };
} // namespace std
