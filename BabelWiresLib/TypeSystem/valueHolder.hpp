/**
 * A ValueHolder is a container which holds a single value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/editableValue.hpp>
#include <BabelWiresLib/TypeSystem/value.hpp>

#include <variant>

namespace babelwires {
    template <typename VALUE> class NewValueHolderTemplate;

    /// A ValueHolderTemplate is a container which holds a single value.
    /// TODO in-place storage for small values.
    template <typename VALUE> class ValueHolderTemplate {
      public:
        ValueHolderTemplate() = default;
        ValueHolderTemplate(const ValueHolderTemplate& other);
        ValueHolderTemplate(ValueHolderTemplate&& other);
        /// An assert checks the type at runtime.
        template <typename OTHER> ValueHolderTemplate(const ValueHolderTemplate<OTHER>& other);
        template <typename OTHER> ValueHolderTemplate(ValueHolderTemplate<OTHER>&& other);

        // TODO Remove this constructor: It makes it far too easy to do a shallow clone.
        ValueHolderTemplate(const VALUE& value);
        ValueHolderTemplate(VALUE&& value);
        ValueHolderTemplate(std::unique_ptr<VALUE> ptr);

        ValueHolderTemplate& operator=(const ValueHolderTemplate& other);
        ValueHolderTemplate& operator=(ValueHolderTemplate&& other);

        ValueHolderTemplate& operator=(const VALUE& value);
        ValueHolderTemplate& operator=(VALUE&& value);
        ValueHolderTemplate& operator=(std::unique_ptr<VALUE> ptr);
        ValueHolderTemplate& operator=(std::shared_ptr<const VALUE> ptr);

        /// Construct a value of Value subclass VALUE. The returned NewValueHolder can be used as a
        /// ValueHolder, but carries a non-const reference to the new value. The non-const reference
        /// can be used to mutate the new value but that must be done before the ValueHolder is
        /// made available outside the current context.
        template <typename T, typename... ARGS> static NewValueHolderTemplate<VALUE> makeValue(ARGS&&... args);

        /// Is this currently holding anything?
        operator bool() const;

        /// No longer hold anything.
        void clear();

        const VALUE& operator*() const;
        const VALUE* operator->() const;

        void swap(ValueHolderTemplate& other);

        /// Clone the contents to ensure they are not shared and return a non-const pointer.
        /// Any manipulations must be performed before the ValueHolder is shared.
        VALUE& copyContentsAndGetNonConst();

        friend bool operator==(const ValueHolderTemplate& a, const ValueHolderTemplate& b) {
            return (a.m_pointerToValue == b.m_pointerToValue) ||
                   (a.m_pointerToValue && b.m_pointerToValue && (*a.m_pointerToValue == *b.m_pointerToValue));
        }
        friend bool operator==(const ValueHolderTemplate& a, const Value* b) {
            return (a.m_pointerToValue.get() == b) || (a.m_pointerToValue && b && (*a.m_pointerToValue == *b));
        }
        friend bool operator==(const Value* a, const ValueHolderTemplate& b) {
            return (a == b.m_pointerToValue.get()) || (a && b.m_pointerToValue && (*a == *b.m_pointerToValue));
        }
        friend bool operator!=(const ValueHolderTemplate& a, const ValueHolderTemplate& b) { return !(a == b); }
        friend bool operator!=(const ValueHolderTemplate& a, const Value* b) { return !(a == b); }
        friend bool operator!=(const Value* a, const ValueHolderTemplate& b) { return !(a == b); }

        /// Use by ValueHolder to visit the value only if necessary.
        // TODO Having this here is ugly (same for filePaths). ValueHolder and ValueHolder probably
        // need a better implementation.
        void visitIdentifiers(IdentifierVisitor& visitor);

        /// Use by ValueHolder to visit the value only if necessary.
        void visitFilePaths(FilePathVisitor& visitor);

        /// Allows implicit assignment to related const ValueHolders, asserting that the type matches.
        /// The implementation reinterprets this, which is safe because the storage does not
        /// depend on the template argument.
        template <typename OTHER> operator const ValueHolderTemplate<OTHER>&() const;

        /// Not recommended, but can be used to get a pointer to the contained value.
        /// This is unsafe only in the sense that a caller might be tempted to keep the pointer.
        /// This is highly likely to dangle if the value is modified, so DO NOT KEEP IT.
        const VALUE* getUnsafe() const;

      private:
        /// The object is not cloned. Since a caller could easily have kept a non-const shared pointer to the object, we
        /// make this constructor private.
        ValueHolderTemplate(std::shared_ptr<const VALUE> ptr);

      private:
        using PointerToValue = std::shared_ptr<const Value>;
        PointerToValue m_pointerToValue;
    };

    /// The return value of ValueHolder::makeValue which can be treated as a ValueHolder&& but
    /// also provides non-const access to the new value.
    template <typename VALUE> class NewValueHolderTemplate {
      public:
        ValueHolderTemplate<Value> m_valueHolder;
        VALUE& m_nonConstReference;
        operator ValueHolderTemplate<Value>&&() && { return std::move(m_valueHolder); }
        template <typename DERIVED> NewValueHolderTemplate<DERIVED> is() && {
            return NewValueHolderTemplate<DERIVED>{std::move(m_valueHolder),
                                                   m_nonConstReference.template is<DERIVED>()};
        }
    };

    template <typename VALUE>
    template <typename T, typename... ARGS>
    NewValueHolderTemplate<VALUE> ValueHolderTemplate<VALUE>::makeValue(ARGS&&... args) {
        auto sharedPtr = std::make_shared<T>(std::forward<ARGS>(args)...);
        T& ref = *sharedPtr;
        return NewValueHolderTemplate<Value>{ValueHolderTemplate<Value>(std::shared_ptr<const T>(std::move(sharedPtr))),
                                             ref};
    }

    using ValueHolder = ValueHolderTemplate<Value>;
    using NewValueHolder = NewValueHolderTemplate<Value>;
} // namespace babelwires

namespace std {
    template <typename VALUE> struct hash<babelwires::ValueHolderTemplate<VALUE>> {
        inline std::size_t operator()(const babelwires::ValueHolder& valueHolder) const {
            return valueHolder->getHash();
        }
    };
} // namespace std

#include <BabelWiresLib/TypeSystem/valueHolder_inl.hpp>