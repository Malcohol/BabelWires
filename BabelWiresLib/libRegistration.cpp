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
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumAtomTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumUnionTypeConstructor.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>
#include <BabelWiresLib/Types/File/fileTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/Sum/sumTypeConstructor.hpp>
#include <BabelWiresLib/Types/Tuple/tupleTypeConstructor.hpp>

#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

void babelwires::registerLib(babelwires::ProjectContext& context) {
    context.m_typeSystem.addEntry<DefaultIntType>();
    context.m_typeSystem.addEntry<NonNegativeIntType>();
    context.m_typeSystem.addEntry<StringType>();
    context.m_typeSystem.addEntry<DefaultRationalType>();
    context.m_typeSystem.addEntry<MapEntryFallbackKind>();
    context.m_typeSystem.addEntry<FailureType>();
    context.m_typeSystem.addTypeConstructor<EnumAtomTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<EnumUnionTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<IntTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<RationalTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<SumTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<MapTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<ArrayTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<FileTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<TupleTypeConstructor>();
}
