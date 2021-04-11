/**
 * Implementation code for the field name macros.
 *
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include "BabelWires/Features/Path/fieldName.hpp"

babelwires::RegisteredFieldIdentifiers
babelwires::detail::getFieldIdentifiers(const babelwires::FieldIdentifiersSource& source) {
    RegisteredFieldIdentifiers identifiers;
    babelwires::FieldNameRegistry::WriteAccess fieldNameRegistry = babelwires::FieldNameRegistry::write();
    identifiers.reserve(source.size());
    for (const auto& t : source) {
        identifiers.emplace_back(fieldNameRegistry->addFieldName(std::get<0>(t), std::get<1>(t), std::get<2>(t),
                                                                 FieldNameRegistry::Authority::isAuthoritative));
    };
    return identifiers;
}

bool babelwires::detail::testFieldIdentifiers(const babelwires::FieldIdentifiersSource& source,
                                              const babelwires::RegisteredFieldIdentifiers& ids) {
    babelwires::FieldNameRegistry::ReadAccess fieldNameRegistry = babelwires::FieldNameRegistry::read();
    auto sourceIt = source.begin();
    auto idsIt = ids.begin();
    while ((sourceIt != source.end()) && (idsIt != ids.end())) {
        if (babelwires::FieldNameRegistry::read()->getFieldName(*idsIt) != std::get<1>(*sourceIt)) {
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
