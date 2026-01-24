/**
 * A ValueHolder is a container which holds a single value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BaseLib/DataContext/filePathVisitor.hpp>
#include <BaseLib/Identifiers/identifierVisitor.hpp>

#include <BabelWiresLib/TypeSystem/value.hpp>

#include <memory>

namespace babelwires {
    /// Forward declare NewValueHolder used by ValueHolder::makeValue
    template <typename T> class NewValueHolderTemplate;

    /// A ValueHolder is a container which holds a single Value.
    /// The held value will be immutable throughout its lifetime.
    /// TODO in-place storage for small values.
    class ValueHolder {
      public:
        ValueHolder() = default;
        ValueHolder(const ValueHolder& other);
        ValueHolder(ValueHolder&& other);
        ValueHolder(Value&& value);
        template <typename VALUE> ValueHolder(std::unique_ptr<VALUE> ptr);

        /// It's too easy to call this by accident, triggering an unnecessary clone.
        /// When required, the caller can clone or move the value.
        ValueHolder(const Value&) = delete;

        ValueHolder& operator=(const ValueHolder& other);
        ValueHolder& operator=(ValueHolder&& other);
        ValueHolder& operator=(Value&& value);
        template <typename VALUE> ValueHolder& operator=(std::unique_ptr<VALUE> ptr);

        /// Deleted to ensure the caller makes a choice between cloning the value
        /// or moving it.
        ValueHolder& operator=(const Value& value) = delete;

        /// Construct a value of Value subclass T. The returned NewValueHolder can be used as a
        /// ValueHolder, but carries a non-const reference to the new value. The non-const reference
        /// can be used to mutate the new value but that must be done before the ValueHolder is
        /// made available outside the current context.
        template <typename T, typename... ARGS> static NewValueHolderTemplate<T> makeValue(ARGS&&... args);

        /// Is this currently holding anything?
        operator bool() const;

        /// No longer hold anything.
        void clear();

        const Value& operator*() const;
        const Value* operator->() const;

        void swap(ValueHolder& other);

        /// Shallow clone the contents to ensure they are not shared and return a non-const pointer.
        /// Any manipulations must be performed before the ValueHolder is further shared.
        Value& copyContentsAndGetNonConst();

        friend bool operator==(const ValueHolder& a, const ValueHolder& b) {
            return (a.m_pointerToValue == b.m_pointerToValue) ||
                   (a.m_pointerToValue && b.m_pointerToValue && (*a.m_pointerToValue == *b.m_pointerToValue));
        }
        friend bool operator==(const ValueHolder& a, const Value* b) {
            return (a.m_pointerToValue.get() == b) || (a.m_pointerToValue && b && (*a.m_pointerToValue == *b));
        }
        friend bool operator==(const Value* a, const ValueHolder& b) {
            return (a == b.m_pointerToValue.get()) || (a && b.m_pointerToValue && (*a == *b.m_pointerToValue));
        }
        friend bool operator!=(const ValueHolder& a, const ValueHolder& b) { return !(a == b); }
        friend bool operator!=(const ValueHolder& a, const Value* b) { return !(a == b); }
        friend bool operator!=(const Value* a, const ValueHolder& b) { return !(a == b); }

        /// Used by ValueHolder to visit the value only if necessary.
        // TODO Having this here is ugly (same for filePaths). ValueHolder and ValueHolder probably
        // need a better implementation.
        void visitIdentifiers(IdentifierVisitor& visitor);

        /// Used by ValueHolder to visit the value only if necessary.
        void visitFilePaths(FilePathVisitor& visitor);

        /// Not recommended, but can be used to get a pointer to the contained value.
        /// This is unsafe only in the sense that a caller might be tempted to keep the pointer.
        /// This is highly likely to dangle if the value is modified, so DO NOT KEEP IT.
        const Value* getUnsafe() const;

      private:
        /// Internal constructor called by makeValue.
        template<typename VALUE>
        ValueHolder(std::shared_ptr<VALUE> ptr);

      private:
        using PointerToValue = std::shared_ptr<const Value>;
        PointerToValue m_pointerToValue;
    };

    /// The return value of ValueHolder::makeValue which can be treated as a ValueHolder&& but
    /// also provides non-const access to the new value.
    template <typename T> class NewValueHolderTemplate {
      public:
        ValueHolder m_valueHolder;
        T& m_nonConstReference;
        operator ValueHolder() && { return std::move(m_valueHolder); }
        operator NewValueHolderTemplate<Value>() && { return {std::move(m_valueHolder), m_nonConstReference}; }
    };

    using NewValueHolder = NewValueHolderTemplate<Value>;
} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::ValueHolder> {
        inline std::size_t operator()(const babelwires::ValueHolder& valueHolder) const {
            const ::babelwires::Value* v = valueHolder.getUnsafe();
            return v ? v->getHash() : 0;
        }
    };
} // namespace std

#include <BabelWiresLib/TypeSystem/valueHolder_inl.hpp>