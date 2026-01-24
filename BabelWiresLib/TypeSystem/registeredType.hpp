/**
 * A registered type can be directly referenced using an RegisteredTypeId.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include <BabelWiresLib/TypeSystem/typeExp.hpp>

#include <BaseLib/Identifiers/registeredIdentifier.hpp>

/// This macro is exists primarily for testing. The REGISTERED_TYPE macro should almost always be used instead.
#define REGISTERED_TYPE_WITH_REGISTERED_ID(IDENTIFIER, VERSION)                                                        \
    static babelwires::RegisteredTypeId getThisIdentifier() {                                                          \
        return IDENTIFIER;                                                                                             \
    }                                                                                                                  \
    static babelwires::VersionNumber getVersion() {                                                                    \
        return VERSION;                                                                                                \
    }

/// A registered type can be directly referenced using a RegisteredTypeId.
/// The TypeSystem expects them to support certain functions and methods, which
/// this macro provides.
#define REGISTERED_TYPE(IDENTIFIER, NAME, UUID, VERSION)                                                               \
    REGISTERED_TYPE_WITH_REGISTERED_ID(BW_MEDIUM_ID(IDENTIFIER, NAME, UUID), VERSION)
