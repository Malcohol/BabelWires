/**
 * A type describes a valid set of values.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeRef.hpp>

#include <Common/Identifiers/registeredIdentifier.hpp>

/// Intended mainly for testing.
#define PRIMITIVE_TYPE_WITH_REGISTERED_ID(IDENTIFIER, VERSION)                                                         \
    static babelwires::LongIdentifier getThisIdentifier() { return IDENTIFIER; }                                       \
    static babelwires::VersionNumber getVersion() { return VERSION; }                                                  \
    babelwires::TypeRef getTypeRef() const override { return getThisIdentifier(); }

/// Primitive types are not derived from other types, so they need to be registered.
/// The TypeSystem expects primitive types to support certain functions and methods, which
/// this macro provides.
#define PRIMITIVE_TYPE(IDENTIFIER, NAME, UUID, VERSION)                                                                \
    PRIMITIVE_TYPE_WITH_REGISTERED_ID(REGISTERED_LONGID(IDENTIFIER, NAME, UUID), VERSION)
