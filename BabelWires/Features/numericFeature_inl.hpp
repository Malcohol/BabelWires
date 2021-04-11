/**
 * ValueFeatures carrying numeric values, e.g. Integers and Rationals.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <functional>

template <typename T> T babelwires::NumericFeature<T>::get() const {
    return m_value;
}

template <typename T> void babelwires::NumericFeature<T>::set(T value) {
    if (!getRange().contains(value)) {
        throw ModelException() << "Attempting to set a NumericFeature with an out-of-range value " << value;
    }
    if (value != m_value) {
        setChanged(Changes::ValueChanged);
        m_value = value;
    }
}

template <typename T> babelwires::Range<T> babelwires::NumericFeature<T>::getRange() const {
    // The min and max value of T.
    return Range<T>();
}

template <typename T> void babelwires::NumericFeature<T>::doSetToDefault() {
    const Range<T> r = getRange();
    if ((r.m_min <= 0) && (r.m_max >= 0)) {
        set(0);
    } else {
        set(r.m_min);
    }
}

template <typename T> void babelwires::NumericFeature<T>::doAssign(const ValueFeature& other) {
    const NumericFeature<T>& otherNum = dynamic_cast<const NumericFeature<T>&>(other);
    set(otherNum.get());
}

template <typename T> std::size_t babelwires::NumericFeature<T>::doGetHash() const {
    return std::hash<T>{}(m_value);
}
