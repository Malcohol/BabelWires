/**
 * Templates which add standard functionality to features, used via CRTP (Curiously Recurring Template Pattern).
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
template <typename T, int MIN, int MAX>
babelwires::Range<unsigned int> babelwires::HasStaticSizeRange<T, MIN, MAX>::doGetSizeRange() const {
    return Range<unsigned int>(MIN, MAX);
}
