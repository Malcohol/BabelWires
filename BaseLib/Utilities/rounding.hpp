/**
 * Some utilities for Unicode
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <concepts>
#include <cmath>

namespace babelwires {
    /// Round a floating point value to the specified number of decimal places.
    template<typename T> requires std::is_floating_point_v<T>
    const inline T roundTo(T value, int decimalPlaces) { 
        const T factor = std::pow(static_cast<T>(10.0), static_cast<T>(decimalPlaces));
        return std::round(value * factor) / factor; 
    }

}
