/**
 * Templates which add standard functionality to features, used via CRTP (Curiously Recurring Template Pattern).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename T, int LOW, int HIGH>
babelwires::Range<typename T::ValueType> babelwires::HasStaticRange<T, LOW, HIGH>::getRange() const {
    return Range<typename T::ValueType>(LOW, HIGH);
}

template <typename T, int DEFAULT> void babelwires::HasStaticDefault<T, DEFAULT>::doSetToDefault() {
    T::set(DEFAULT);
}

template <typename T, int MIN, int MAX>
babelwires::Range<unsigned int> babelwires::HasStaticSizeRange<T, MIN, MAX>::doGetSizeRange() const {
    return Range<unsigned int>(MIN, MAX);
}
