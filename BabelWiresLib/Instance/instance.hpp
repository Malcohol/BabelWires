/**
 * Common header for standard instance functionality.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

// The project manages entirely using the generic operations of the type/value and feature systems.
// However, this is inconvenient for code in processors or import/export code which knows the structure
// of the data involved. The instance system allows such code to explore hierarchies of ValueFeatures
// using explicitly typed wrapper classes.

// TODO Describe usage here

// clang-format off
#include <BabelWiresLib/Instance/instanceTemplates.hpp>
#include <BabelWiresLib/Instance/instanceDSL.hpp>

#include <BabelWiresLib/Instance/enumTypeInstance.hpp>
#include <BabelWiresLib/Instance/arrayTypeInstance.hpp>
#include <BabelWiresLib/Instance/intTypeInstance.hpp>
#include <BabelWiresLib/Instance/mapTypeInstance.hpp>
#include <BabelWiresLib/Instance/rationalTypeInstance.hpp>
#include <BabelWiresLib/Instance/stringTypeInstance.hpp>
#include <BabelWiresLib/Instance/recordWithVariantsTypeInstance.hpp>
// clang-format on