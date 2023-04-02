/**
 * A ValueHolder is a container which holds a single value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/value.hpp>

#include <variant>

namespace babelwires {
    class NewValueHolder;

    /// A ValueHolder is a container which holds a single value.
    /// TODO in-place storage for small values.
    class ValueHolder : public ProjectVisitable {
      public:
        ValueHolder() = default;
        ValueHolder(const ValueHolder& other);
        ValueHolder(ValueHolder&& other);

        ValueHolder(const Value& value);
        ValueHolder(Value&& value);
        ValueHolder(std::unique_ptr<Value> ptr);
        ValueHolder(std::shared_ptr<const Value> ptr);

        ValueHolder& operator=(const ValueHolder& other);
        ValueHolder& operator=(ValueHolder&& other);

        ValueHolder& operator=(const Value& value);
        ValueHolder& operator=(Value&& value);
        ValueHolder& operator=(std::unique_ptr<Value> ptr);
        ValueHolder& operator=(std::shared_ptr<const Value> ptr);

        /// Construct a value of Value subclass T. The returned NewValueHolder carries a non-const
        /// reference to the new value. The non-const reference can be used to mutate the new value
        /// but that must be done before the ValueHolder is made available outside the current context.
        template <typename T, typename... ARGS> static NewValueHolder makeValue(ARGS&&... args);

        operator bool() const;
        const Value& operator*() const;
        const Value* operator->() const;

        /// Clone the contents to ensure they are not shared and return a non-const pointer.
        /// Any manipulations must be performed before the ValueHolder is shared.
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

        void visitIdentifiers(IdentifierVisitor& visitor) override;
        void visitFilePaths(FilePathVisitor& visitor) override;

      private:
        using PointerToValue = std::shared_ptr<const Value>;
        PointerToValue m_pointerToValue;
    };

    class NewValueHolder {
      public:
        ValueHolder m_valueHolder;
        Value& m_nonConstReference;
        operator ValueHolder&&() && { return std::move(m_valueHolder); }
    };

    template <typename T, typename... ARGS> NewValueHolder ValueHolder::makeValue(ARGS&&... args) {
        auto sharedPtr = std::make_shared<T>(std::forward<ARGS>(args)...);
        T& ref = *sharedPtr;
        return NewValueHolder{ValueHolder(std::shared_ptr<const T>(std::move(sharedPtr))), ref};
    }

} // namespace babelwires

namespace std {
    template <> struct hash<babelwires::ValueHolder> {
        inline std::size_t operator()(const babelwires::ValueHolder& valueHolder) const {
            return valueHolder->getHash();
        }
    };
} // namespace std
