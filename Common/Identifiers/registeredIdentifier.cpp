/**
 * Implementation code for the field name macros.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "Common/Identifiers/registeredIdentifier.hpp"

babelwires::RegisteredIdentifiers
babelwires::detail::getIdentifiers(const babelwires::IdentifiersSource& source) {
    RegisteredIdentifiers identifiers;
    babelwires::IdentifierRegistry::WriteAccess identifierRegistry = babelwires::IdentifierRegistry::write();
    identifiers.reserve(source.size());
    for (const auto& t : source) {
        identifiers.emplace_back(identifierRegistry->addIdentifierWithMetadata(std::get<0>(t), std::get<1>(t), std::get<2>(t),
                                                                 IdentifierRegistry::Authority::isAuthoritative));
    };
    return identifiers;
}

bool babelwires::detail::testIdentifiers(const babelwires::IdentifiersSource& source,
                                              const babelwires::RegisteredIdentifiers& ids) {
    babelwires::IdentifierRegistry::ReadAccess identifierRegistry = babelwires::IdentifierRegistry::read();
    auto sourceIt = source.begin();
    auto idsIt = ids.begin();
    while ((sourceIt != source.end()) && (idsIt != ids.end())) {
        if (babelwires::IdentifierRegistry::read()->getName(*idsIt) != std::get<1>(*sourceIt)) {
            return false;
        }
        ++sourceIt;
        ++idsIt;
    }
    if (sourceIt != source.end()) {
        return false;
    }
    if (idsIt != ids.end()) {
        return false;
    }
    return true;
}
