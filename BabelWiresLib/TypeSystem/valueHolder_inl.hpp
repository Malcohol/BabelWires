/**
 * A ValueHolder is a container which holds a single value.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

inline babelwires::ValueHolder::ValueHolder(const ValueHolder& other)
    : m_pointerToValue(other.m_pointerToValue) {}

inline babelwires::ValueHolder::ValueHolder(ValueHolder&& other)
    : m_pointerToValue(std::move(other.m_pointerToValue)) {}

inline babelwires::ValueHolder::ValueHolder(Value&& value)
    : m_pointerToValue(std::move(value).cloneShared()) {}

template <typename VALUE>
babelwires::ValueHolder::ValueHolder(std::unique_ptr<VALUE> ptr)
    : m_pointerToValue(ptr.release()) {}

template <typename VALUE>
babelwires::ValueHolder::ValueHolder(std::shared_ptr<VALUE> ptr)
    : m_pointerToValue(std::move(ptr)) {}

inline babelwires::ValueHolder& babelwires::ValueHolder::operator=(const ValueHolder& other) {
    m_pointerToValue = other.m_pointerToValue;
    return *this;
}

inline babelwires::ValueHolder& babelwires::ValueHolder::operator=(ValueHolder&& other) {
    m_pointerToValue = std::move(other.m_pointerToValue);
    return *this;
}

inline babelwires::ValueHolder& babelwires::ValueHolder::operator=(Value&& value) {
    m_pointerToValue = std::move(value).cloneShared();
    return *this;
}

template <typename VALUE> babelwires::ValueHolder& babelwires::ValueHolder::operator=(std::unique_ptr<VALUE> ptr) {
    m_pointerToValue = std::shared_ptr<const Value>(ptr.release());
    return *this;
}

inline babelwires::ValueHolder::operator bool() const {
    return m_pointerToValue.get();
}

inline void babelwires::ValueHolder::clear() {
    m_pointerToValue = nullptr;
}

inline const babelwires::Value& babelwires::ValueHolder::operator*() const {
    return m_pointerToValue->is<Value>();
}

inline const babelwires::Value* babelwires::ValueHolder::operator->() const {
    return &m_pointerToValue->is<Value>();
}

inline void babelwires::ValueHolder::swap(ValueHolder& other) {
    m_pointerToValue.swap(other.m_pointerToValue);
}

template <typename T, typename... ARGS>
babelwires::NewValueHolderTemplate<T> babelwires::ValueHolder::makeValue(ARGS&&... args) {
    auto sharedPtr = std::make_shared<T>(std::forward<ARGS>(args)...);
    T& ref = *sharedPtr;
    return NewValueHolderTemplate<T>{std::move(sharedPtr), ref};
}
