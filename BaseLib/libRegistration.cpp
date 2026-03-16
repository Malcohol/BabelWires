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
#include <BaseLib/Serialization/deserializationRegistry.hpp>

#include <cassert>

void babelwires::baseLib::registerLib(DataContext& context) {
    context.m_deserializationReg.registerClass<IdentifierRegistry>();
}
