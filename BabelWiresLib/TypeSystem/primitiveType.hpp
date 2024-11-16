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
    static babelwires::PrimitiveTypeId getThisIdentifier() {                                                           \
        return IDENTIFIER;                                                                                             \
    }                                                                                                                  \
    static babelwires::TypeRef getThisType() {                                                                         \
        return getThisIdentifier();                                                                                    \
    }                                                                                                                  \
    static babelwires::VersionNumber getVersion() {                                                                    \
        return VERSION;                                                                                                \
    }                                                                                                                  \
    babelwires::TypeRef getTypeRef() const override {                                                                  \
        return getThisIdentifier();                                                                                    \
    }

/// Primitive types (i.e. types which are not constructed from other types) need to be directly
/// registered in the TypeSystem.
/// The TypeSystem expects them to support certain functions and methods, which
/// this macro provides.
#define PRIMITIVE_TYPE(IDENTIFIER, NAME, UUID, VERSION)                                                                \
    PRIMITIVE_TYPE_WITH_REGISTERED_ID(BW_MEDIUM_ID(IDENTIFIER, NAME, UUID), VERSION)
