/**
 * Provides a representation of time useful for timestamps.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/

#pragma once

#include <chrono>

namespace babelwires {
    /// A representation of time. This is not a wallclock time,
    /// so it would need conversion before it was shown to the user.
    using CommandTimestamp = decltype(std::chrono::steady_clock::now());
} // namespace babelwires
