/**
 * Registration of factories etc for the main BabelWiresLib
 *
 * (C) 2021 Malcolm Tyrrell
 *
 * Licensed under the GPLv3.0. See LICENSE file.
 **/
#include <BabelWiresLib/libRegistration.hpp>

#include <BabelWiresLib/Project/Modifiers/arraySizeModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/connectionModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/selectOptionalsModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/selectRecordVariantModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/setTypeVariableModifierData.hpp>
#include <BabelWiresLib/Project/Modifiers/valueAssignmentData.hpp>
#include <BabelWiresLib/Project/Nodes/ProcessorNode/processorNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/SourceFileNode/sourceFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/TargetFileNode/targetFileNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/ValueNode/valueNodeData.hpp>
#include <BabelWiresLib/Project/Nodes/nodeData.hpp>
#include <BabelWiresLib/Project/projectData.hpp>
#include <BabelWiresLib/ProjectExtra/projectDataLocation.hpp>
#include <BabelWiresLib/Serialization/projectBundle.hpp>
#include <BabelWiresLib/TypeSystem/typeExp.hpp>
#include <BabelWiresLib/TypeSystem/typeSystem.hpp>
#include <BabelWiresLib/Types/Array/arrayTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumAtomTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumUnionTypeConstructor.hpp>
#include <BabelWiresLib/Types/Enum/enumValue.hpp>
#include <BabelWiresLib/Types/Failure/failureType.hpp>
#include <BabelWiresLib/Types/File/fileTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/genericTypeConstructor.hpp>
#include <BabelWiresLib/Types/Generic/typeVariableTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/Types/Int/intValue.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToOneFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/allToSameFallbackMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/mapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapEntries/oneToOneMapEntryData.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapProjectDataLocation.hpp>
#include <BabelWiresLib/Types/Map/MapProject/mapSerialization.hpp>
#include <BabelWiresLib/Types/Map/mapTypeConstructor.hpp>
#include <BabelWiresLib/Types/Map/mapValue.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>
#include <BabelWiresLib/Types/Rational/rationalValue.hpp>
#include <BabelWiresLib/Types/Record/fieldIdValue.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/Record/recordTypeConstructor.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/String/stringValue.hpp>
#include <BabelWiresLib/Types/Sum/sumTypeConstructor.hpp>
#include <BabelWiresLib/Types/Tuple/tupleTypeConstructor.hpp>
#include <BabelWiresLib/Types/Tuple/tupleValue.hpp>

#include <BaseLib/Context/context.hpp>
#include <BaseLib/Serialization/deserializationRegistry.hpp>

#include <cassert>

void babelwires::registerLib(babelwires::Context& context) {
    context.getService<TypeSystem>().addType<DefaultIntType>();
    context.getService<TypeSystem>().addType<NonNegativeIntType>();
    context.getService<TypeSystem>().addType<StringType>();
    context.getService<TypeSystem>().addType<DefaultRationalType>();
    context.getService<TypeSystem>().addType<MapEntryFallbackKind>();
    context.getService<TypeSystem>().addType<FailureType>(context.getService<TypeSystem>());
    context.getService<TypeSystem>().addTypeConstructor<EnumAtomTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<EnumUnionTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<IntTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<RationalTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<SumTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<MapTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<ArrayTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<FileTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<TupleTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<RecordTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<GenericTypeConstructor>();
    context.getService<TypeSystem>().addTypeConstructor<TypeVariableTypeConstructor>();

    context.getService<DeserializationRegistry>().registerClass<ProjectData>();
    context.getService<DeserializationRegistry>().registerClass<ProjectDataLocation>();
    context.getService<DeserializationRegistry>().registerClass<ProjectBundle>();
    context.getService<DeserializationRegistry>().registerClass<TypeExp>();
    context.getService<DeserializationRegistry>().registerClass<UiData>();
    context.getService<DeserializationRegistry>().registerClass<SourceFileNodeData>();
    context.getService<DeserializationRegistry>().registerClass<TargetFileNodeData>();
    context.getService<DeserializationRegistry>().registerClass<ProcessorNodeData>();
    context.getService<DeserializationRegistry>().registerClass<ValueNodeData>();
    context.getService<DeserializationRegistry>().registerClass<ArraySizeModifierData>();
    context.getService<DeserializationRegistry>().registerClass<ConnectionModifierData>();
    context.getService<DeserializationRegistry>().registerClass<ValueAssignmentData>();
    context.getService<DeserializationRegistry>().registerClass<SelectOptionalsModifierData>();
    context.getService<DeserializationRegistry>().registerClass<SelectRecordVariantModifierData>();
    context.getService<DeserializationRegistry>().registerClass<SetTypeVariableModifierData>();
    context.getService<DeserializationRegistry>().registerClass<IntValue>();
    context.getService<DeserializationRegistry>().registerClass<StringValue>();
    context.getService<DeserializationRegistry>().registerClass<RationalValue>();
    context.getService<DeserializationRegistry>().registerClass<EnumValue>();
    context.getService<DeserializationRegistry>().registerClass<TupleValue>();
    context.getService<DeserializationRegistry>().registerClass<MapValue>();
    context.getService<DeserializationRegistry>().registerClass<FieldIdValue>();
    context.getService<DeserializationRegistry>().registerClass<OneToOneMapEntryData>();
    context.getService<DeserializationRegistry>().registerClass<AllToOneFallbackMapEntryData>();
    context.getService<DeserializationRegistry>().registerClass<AllToSameFallbackMapEntryData>();
    context.getService<DeserializationRegistry>().registerClass<MapBundle>();
    context.getService<DeserializationRegistry>().registerClass<MapProjectDataLocation>();
}
