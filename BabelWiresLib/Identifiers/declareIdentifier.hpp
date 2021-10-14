/**
 * The macros in this file allow field names to be implicitly registered on their first use.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#pragma once

#include "BabelWiresLib/Identifiers/identifierRegistry.hpp"
#include <cassert>

namespace babelwires {
    /// See the FIELD_NAME_VECTOR macro.
    using FieldIdentifiersSource = std::vector<std::tuple<const babelwires::FieldIdentifier, std::string, Uuid>>;

    /// See the FIELD_NAME_VECTOR macro.
    using RegisteredFieldIdentifiers = std::vector<babelwires::FieldIdentifier>;

    namespace detail {
        RegisteredFieldIdentifiers getFieldIdentifiers(const FieldIdentifiersSource& source);
        bool testFieldIdentifiers(const FieldIdentifiersSource& source,
                                  const babelwires::RegisteredFieldIdentifiers& ids);
    } // namespace detail
} // namespace babelwires

/// This macro offers a convenient way of registering field identifiers.
/// This expression evaluates to a FieldIdentifier which has the data from the given IDENTIFIER and a discriminator
/// which allows the name to be looked up in the FieldNameRegistry. The registration happens only the first time it is
/// called.
#define FIELD_NAME(IDENTIFIER, NAME, UUID)                                                                             \
    ([](auto&& id, auto&& name, auto&& uuid) {                                                                         \
        static babelwires::FieldIdentifier f = babelwires::FieldNameRegistry::write()->addFieldName(                   \
            id, name, uuid, babelwires::FieldNameRegistry::Authority::isAuthoritative);                                \
        assert(                                                                                                        \
            (babelwires::FieldNameRegistry::read()->getFieldName(f) == name) &&                                        \
            "Each usage of this macro should register a single fieldIdentifier."                                       \
            " Do not use it in cases where the same code can be called a second time with a different identifier.");   \
        return f;                                                                                                      \
    }(IDENTIFIER, NAME, UUID))

/// This macro offers a convenient way of registering a vector of field identifiers.
/// This expression evaluates to a const RegisteredFieldIdentifiers& vector, which has been populated from the
/// FieldIdentifiersSource array. The registration happens only the first time it is called.
#define FIELD_NAME_VECTOR(SOURCE)                                                                                      \
    ([](auto&& source) -> const babelwires::RegisteredFieldIdentifiers& {                                              \
        static babelwires::RegisteredFieldIdentifiers ids = babelwires::detail::getFieldIdentifiers(source);           \
        assert(babelwires::detail::testFieldIdentifiers(source, ids) &&                                                \
               "Each usage of this macro must take the same source each time it is run."                               \
               " Do not use it in cases where the same code can be called a second time with a different contents.");  \
        return ids;                                                                                                    \
    }(SOURCE))
