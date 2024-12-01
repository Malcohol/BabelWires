/**
 * Classes defining graphical position and size.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <cstdint>
#include <functional>

namespace babelwires {

    typedef std::int32_t UiCoord;

    /// Defines the graphical position of a Node.
    /// We put this in the BabelWires library, because it simplifies serialization.
    struct UiPosition {
        UiCoord m_x = 0;
        UiCoord m_y = 0;

        bool operator!=(const UiPosition& other) const;
        bool operator==(const UiPosition& other) const;

        void operator+=(UiCoord offset);
        void operator+=(const UiPosition& other);

        UiPosition operator+(UiCoord offset) const;
        UiPosition operator+(const UiPosition& other) const;

        UiPosition operator-() const;

        std::size_t getHash() const;
    };

    struct UiSize {
        bool operator!=(const UiSize& other) const;
        bool operator==(const UiSize& other) const;

        UiCoord m_width = 250;
    };

} // namespace babelwires

namespace std {

    template <> struct hash<babelwires::UiPosition> {
        inline std::size_t operator()(const babelwires::UiPosition& pos) const { return pos.getHash(); }
    };

} // namespace std
