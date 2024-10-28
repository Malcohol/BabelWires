/**
 * Common header for standard instance functionality.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

// The project operates using the operations of the type/value and valueTree systems, which are entirely generic.
// This is inconvenient for "client code" (i.e. code in processors or import/export code) since that code usually
// knows the specific structure of the data involved. The instance system allows such code to explore ValueTrees
// in a typed way, by allowing types to provide type-aware wrapper classes for ValueTreeNodes.

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