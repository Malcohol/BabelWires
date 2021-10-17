/**
 * The macros in this file allow identifiers to be implicitly registered on their first use.
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include <cassert>

namespace babelwires {
    /// See the REGISTERED_ID_VECTOR macro.
    using IdentifiersSource = std::vector<std::tuple<const babelwires::Identifier, std::string, Uuid>>;

    /// See the REGISTERED_ID_VECTOR macro.
    using RegisteredIdentifiers = std::vector<babelwires::Identifier>;

    namespace detail {
        RegisteredIdentifiers getIdentifiers(const IdentifiersSource& source);
        bool testIdentifiers(const IdentifiersSource& source, const babelwires::RegisteredIdentifiers& ids);
    } // namespace detail
} // namespace babelwires

/// This macro offers a convenient way of registering identifiers.
/// This expression evaluates to a Identifier which has the data from the given IDENTIFIER and a discriminator
/// which allows the name to be looked up in the IdentifierRegistry. The registration happens only the first time it is
/// called.
#define REGISTERED_ID(IDENTIFIER, NAME, UUID)                                                                          \
    ([](auto&& id, auto&& name, auto&& uuid) {                                                                         \
        static babelwires::Identifier f = babelwires::IdentifierRegistry::write()->addIdentifierWithMetadata(          \
            id, name, uuid, babelwires::IdentifierRegistry::Authority::isAuthoritative);                               \
        assert(                                                                                                        \
            (babelwires::IdentifierRegistry::read()->getName(f) == name) &&                                            \
            "Each usage of this macro should register a single fieldIdentifier."                                       \
            " Do not use it in cases where the same code can be called a second time with a different identifier.");   \
        return f;                                                                                                      \
    }(IDENTIFIER, NAME, UUID))

/// This macro offers a convenient way of registering a vector of field identifiers.
/// This expression evaluates to a const RegisteredIdentifiers& vector, which has been populated from the
/// IdentifiersSource array. The registration happens only the first time it is called.
#define REGISTERED_ID_VECTOR(SOURCE)                                                                                   \
    ([](auto&& source) -> const babelwires::RegisteredIdentifiers& {                                                   \
        static babelwires::RegisteredIdentifiers ids = babelwires::detail::getIdentifiers(source);                     \
        assert(babelwires::detail::testIdentifiers(source, ids) &&                                                     \
               "Each usage of this macro must take the same source each time it is run."                               \
               " Do not use it in cases where the same code can be called a second time with a different contents.");  \
        return ids;                                                                                                    \
    }(SOURCE))
