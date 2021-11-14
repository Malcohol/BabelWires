/**
 * Provides type definitions for IDs used in the project.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <Common/uuid.hpp>

#include <cstdint>
#include <functional>
namespace babelwires {
    enum { INVALID_ELEMENT_ID = 0 };

    /// Each element in a project is assigned a consecutive ID.
    using ElementId = std::uint16_t;

    /// Each new project is assigned a random ID.
    using ProjectId = Uuid;
} // namespace babelwires
