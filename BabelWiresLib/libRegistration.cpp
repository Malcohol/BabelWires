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
#include <BabelWiresLib/Types/Enum/addBlankToEnum.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>

void babelwires::registerLib(babelwires::ProjectContext& context) {
    context.m_typeSystem.addEntry<DefaultIntType>();
    context.m_typeSystem.addEntry<StringType>();
    context.m_typeSystem.addEntry<DefaultRationalType>();
    context.m_typeSystem.addEntry<MapEntryFallbackKind>();
    context.m_typeSystem.addTypeConstructor<AddBlankToEnum>();
    context.m_typeSystem.addTypeConstructor<IntTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<RationalTypeConstructor>();
}
