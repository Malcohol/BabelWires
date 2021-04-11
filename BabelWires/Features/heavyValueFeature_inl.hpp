/**
 * A feature which carries an immutable, shareable value of T.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename T>
babelwires::HeavyValueFeature<T>::HeavyValueFeature()
    : m_value(std::make_shared<const T>()) {}

template <typename T> const T& babelwires::HeavyValueFeature<T>::get() const {
    return *m_value;
}

template <typename T> void babelwires::HeavyValueFeature<T>::set(T&& newValue) {
    if (newValue != *m_value) {
        m_value = std::make_shared<T>(newValue);
        setChanged(Changes::ValueChanged);
    }
}

template <typename T> void babelwires::HeavyValueFeature<T>::set(const T& newValue) {
    if (newValue != *m_value) {
        m_value = std::make_shared<T>(newValue);
        setChanged(Changes::ValueChanged);
    }
}

template <typename T> void babelwires::HeavyValueFeature<T>::set(std::unique_ptr<T> newValue) {
    if (*newValue != *m_value) {
        m_value = std::shared_ptr<const T>(newValue.release());
        setChanged(Changes::ValueChanged);
    }
}

template <typename T> void babelwires::HeavyValueFeature<T>::doAssign(const ValueFeature& other) {
    assert(dynamic_cast<const HeavyValueFeature<T>*>(&other) && "Other is not a compatible type");
    const babelwires::HeavyValueFeature<T>& otherH = static_cast<const babelwires::HeavyValueFeature<T>&>(other);
    if (otherH.m_value != m_value) {
        if (*otherH.m_value != *m_value) {
            setChanged(Changes::ValueChanged);
        }
        // Assume it's worth sharing the incoming value even if there's no change.
        m_value = otherH.m_value;
    }
}

template <typename T> void babelwires::HeavyValueFeature<T>::doSetToDefault() {
    T newT;
    if (*m_value != newT) {
        m_value = std::make_shared<const T>(std::move(newT));
        setChanged(Changes::ValueChanged);
    }
}

template <typename T> std::size_t babelwires::HeavyValueFeature<T>::doGetHash() const {
    return m_value->getHash();
}
