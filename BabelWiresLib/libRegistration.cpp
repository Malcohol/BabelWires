/**
 * Registration of factories etc for the main BabelWiresLib
 * 
 * (C) 2021 Malcolm Tyrrell
 * 
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/libRegistration.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/TypeSystem/intType.hpp>

void babelwires::registerLib(babelwires::ProjectContext& context) {
    context.m_typeSystem.addEntry(std::make_unique<IntType>());
}
