/**
 * Registration of factories etc for the main BabelWiresLib
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/libRegistration.hpp>

#include <BabelWiresLib/Project/projectContext.hpp>
#include <BabelWiresLib/Project/projectData.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/Modifiers/selectOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/selectRecordVariantModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>
#include <BabelWiresLib/Serialization/projectBundle.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumAtomTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumUnionTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>
#include <BabelWiresLib/Types/File/fileTypeConstructor.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectDataLocation.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapSerialization.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Record/recordTypeConstructor.hpp>
#include <BabelWiresLib/Types/Record/fieldIdValue.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/Types/Sum/sumTypeConstructor.hpp>
#include <BabelWiresLib/Types/Tuple/tupleTypeConstructor.hpp>
#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>

#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BaseLib/Serialization/deserializationRegistry.hpp>

#include <cassert>

void babelwires::registerLib(babelwires::ProjectContext& context) {
    context.m_typeSystem.addType<DefaultIntType>();
    context.m_typeSystem.addType<NonNegativeIntType>();
    context.m_typeSystem.addType<StringType>();
    context.m_typeSystem.addType<DefaultRationalType>();
    context.m_typeSystem.addType<MapEntryFallbackKind>();
    context.m_typeSystem.addType<FailureType>(context.m_typeSystem);
    context.m_typeSystem.addTypeConstructor<EnumAtomTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<EnumUnionTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<IntTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<RationalTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<SumTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<MapTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<ArrayTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<FileTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<TupleTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<RecordTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<GenericTypeConstructor>();
    context.m_typeSystem.addTypeConstructor<TypeVariableTypeConstructor>();

    context.m_deserializationReg.registerClass<ProjectData>();
    context.m_deserializationReg.registerClass<ProjectDataLocation>();
    context.m_deserializationReg.registerClass<ProjectBundle>();
    context.m_deserializationReg.registerClass<TypeExp>();
    context.m_deserializationReg.registerClass<UiData>();
    context.m_deserializationReg.registerClass<SourceFileNodeData>();
    context.m_deserializationReg.registerClass<TargetFileNodeData>();
    context.m_deserializationReg.registerClass<ProcessorNodeData>();
    context.m_deserializationReg.registerClass<ValueNodeData>();
    context.m_deserializationReg.registerClass<ArraySizeModifierData>();
    context.m_deserializationReg.registerClass<ConnectionModifierData>();
    context.m_deserializationReg.registerClass<ValueAssignmentData>();
    context.m_deserializationReg.registerClass<SelectOptionalsModifierData>();
    context.m_deserializationReg.registerClass<SelectRecordVariantModifierData>();
    context.m_deserializationReg.registerClass<SetTypeVariableModifierData>();
    context.m_deserializationReg.registerClass<IntValue>();
    context.m_deserializationReg.registerClass<StringValue>();
    context.m_deserializationReg.registerClass<RationalValue>();
    context.m_deserializationReg.registerClass<EnumValue>();
    context.m_deserializationReg.registerClass<TupleValue>();
    context.m_deserializationReg.registerClass<MapValue>();
    context.m_deserializationReg.registerClass<FieldIdValue>();
    context.m_deserializationReg.registerClass<OneToOneMapEntryData>();
    context.m_deserializationReg.registerClass<AllToOneFallbackMapEntryData>();
    context.m_deserializationReg.registerClass<AllToSameFallbackMapEntryData>();
    context.m_deserializationReg.registerClass<MapBundle>();
    context.m_deserializationReg.registerClass<MapProjectDataLocation>();
}
