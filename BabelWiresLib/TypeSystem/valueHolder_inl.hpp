/**
 * A ValueHolderTemplate is a container which holds a single value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

namespace babelwires {

    // Implementations for concrete ValueHolder
    inline ValueHolder::ValueHolder(const ValueHolder& other)
        : m_pointerToValue(other.m_pointerToValue) {}

    inline ValueHolder::ValueHolder(ValueHolder&& other)
        : m_pointerToValue(std::move(other.m_pointerToValue)) {}

    inline ValueHolder::ValueHolder(const Value& value)
        : m_pointerToValue(value.cloneShared()) {}

    inline ValueHolder::ValueHolder(Value&& value)
        : m_pointerToValue(std::move(value).cloneShared()) {}

    inline ValueHolder::ValueHolder(std::unique_ptr<Value> ptr)
        : m_pointerToValue(ptr.release()) {}

    inline ValueHolder::ValueHolder(std::shared_ptr<const Value> ptr)
        : m_pointerToValue(std::move(ptr)) {}

    inline ValueHolder& ValueHolder::operator=(const ValueHolder& other) {
        m_pointerToValue = other.m_pointerToValue;
        return *this;
    }

    inline ValueHolder& ValueHolder::operator=(ValueHolder&& other) {
        m_pointerToValue = std::move(other.m_pointerToValue);
        return *this;
    }

    inline ValueHolder& ValueHolder::operator=(const Value& value) {
        m_pointerToValue = value.cloneShared();
        return *this;
    }

    inline ValueHolder& ValueHolder::operator=(Value&& value) {
        m_pointerToValue = std::move(value).cloneShared();
        return *this;
    }

    inline ValueHolder& ValueHolder::operator=(std::unique_ptr<Value> ptr) {
        m_pointerToValue = std::shared_ptr<const Value>(ptr.release());
        return *this;
    }

    inline ValueHolder::operator bool() const {
        return m_pointerToValue.get();
    }

    inline void ValueHolder::clear() {
        m_pointerToValue = nullptr;
    }

    inline const Value& ValueHolder::operator*() const {
        return m_pointerToValue->is<Value>();
    }

    inline const Value* ValueHolder::operator->() const {
        return &m_pointerToValue->is<Value>();
    }

    inline void ValueHolder::swap(ValueHolder& other) {
        m_pointerToValue.swap(other.m_pointerToValue);
    }

    template <typename T, typename... ARGS> NewValueHolder ValueHolder::makeValue(ARGS&&... args) {
        auto sharedPtr = std::make_shared<T>(std::forward<ARGS>(args)...);
        T& ref = *sharedPtr;
        return NewValueHolder{ValueHolder(std::shared_ptr<const T>(std::move(sharedPtr))), ref};
    }

} // namespace babelwires
