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

        /// Is this value an editable value?
        /// The default implementation returns nullptr.
        virtual const EditableValue* tryGetAsEditableValue() const;

        /// Is this value an editable value?
        /// The default implementation returns nullptr.
        virtual EditableValue* tryGetAsEditableValue();

        /// Assert that this is an EditableValue and return a const reference to it.
        const EditableValue& getAsEditableValue() const;

        /// Assert that this is an EditableValue and return a reference to it.
        EditableValue& getAsEditableValue();

        bool operator!=(const Value& other) const { return !(*this == other); }
    };

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::Value> {
        inline std::size_t operator()(const babelwires::Value& value) const { return value.getHash(); }
    };
} // namespace std
