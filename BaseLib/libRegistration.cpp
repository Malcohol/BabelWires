/**
 * Registration of factories etc for BaseLib.
 *
 * (C) 2026 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BaseLib/libRegistration.hpp>

#include <BaseLib/DataContext/dataContext.hpp>
#include <BaseLib/Identifiers/identifierRegistry.hpp>
#include <BaseLib/Serialization/explicitDeserializationRegistry.hpp>

#include <cassert>

void babelwires::baseLib::registerLib(DataContext& context) {
    auto* deserializationRegistry = dynamic_cast<ExplicitDeserializationRegistry*>(&context.m_deserializationReg);
    assert(deserializationRegistry && "baseLib::registerLib requires an ExplicitDeserializationRegistry");

    deserializationRegistry->registerClass<IdentifierRegistry>();
}
