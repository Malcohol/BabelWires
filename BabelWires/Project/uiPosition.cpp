/**
 * Classes defining graphical position and size.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Project/uiPosition.hpp"

#include "Common/Utilities/hash.hpp"

bool babelwires::UiPosition::operator==(const UiPosition& other) const {
    return (m_x == other.m_x) && (m_y == other.m_y);
}

bool babelwires::UiPosition::operator!=(const UiPosition& other) const {
    return (m_x != other.m_x) || (m_y != other.m_y);
}

std::size_t babelwires::UiPosition::getHash() const {
    return hash::mixtureOf(m_x, m_y);
}

void babelwires::UiPosition::operator+=(UiCoord offset) {
    m_x += offset;
    m_y += offset;
}

void babelwires::UiPosition::operator+=(const UiPosition& other) {
    m_x += other.m_x;
    m_y += other.m_y;
}

babelwires::UiPosition babelwires::UiPosition::operator+(UiCoord offset) const {
    babelwires::UiPosition likeThis = *this;
    likeThis += offset;
    return likeThis;
}

babelwires::UiPosition babelwires::UiPosition::operator+(const UiPosition& other) const {
    babelwires::UiPosition likeThis = *this;
    likeThis += other;
    return likeThis;
}

babelwires::UiPosition babelwires::UiPosition::operator-() const {
    return {-m_x, -m_y};
}

bool babelwires::UiSize::operator==(const UiSize& other) const {
    return m_width == other.m_width;
}

bool babelwires::UiSize::operator!=(const UiSize& other) const {
    return m_width != other.m_width;
}
