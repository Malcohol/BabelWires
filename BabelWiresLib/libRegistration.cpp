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
    TypeSystem& typeSystem = context.get<TypeSystem>();
    typeSystem.addType<DefaultIntType>();
    typeSystem.addType<NonNegativeIntType>();
    typeSystem.addType<StringType>();
    typeSystem.addType<DefaultRationalType>();
    typeSystem.addType<MapEntryFallbackKind>();
    typeSystem.addType<FailureType>(typeSystem);
    typeSystem.addTypeConstructor<EnumAtomTypeConstructor>();
    typeSystem.addTypeConstructor<EnumUnionTypeConstructor>();
    typeSystem.addTypeConstructor<IntTypeConstructor>();
    typeSystem.addTypeConstructor<RationalTypeConstructor>();
    typeSystem.addTypeConstructor<SumTypeConstructor>();
    typeSystem.addTypeConstructor<MapTypeConstructor>();
    typeSystem.addTypeConstructor<ArrayTypeConstructor>();
    typeSystem.addTypeConstructor<FileTypeConstructor>();
    typeSystem.addTypeConstructor<TupleTypeConstructor>();
    typeSystem.addTypeConstructor<RecordTypeConstructor>();
    typeSystem.addTypeConstructor<GenericTypeConstructor>();
    typeSystem.addTypeConstructor<TypeVariableTypeConstructor>();

    DeserializationRegistry& deserializationRegistry = context.get<DeserializationRegistry>();
    deserializationRegistry.registerClass<ProjectData>();
    deserializationRegistry.registerClass<ProjectDataLocation>();
    deserializationRegistry.registerClass<ProjectBundle>();
    deserializationRegistry.registerClass<TypeExp>();
    deserializationRegistry.registerClass<UiData>();
    deserializationRegistry.registerClass<SourceFileNodeData>();
    deserializationRegistry.registerClass<TargetFileNodeData>();
    deserializationRegistry.registerClass<ProcessorNodeData>();
    deserializationRegistry.registerClass<ValueNodeData>();
    deserializationRegistry.registerClass<ArraySizeModifierData>();
    deserializationRegistry.registerClass<ConnectionModifierData>();
    deserializationRegistry.registerClass<ValueAssignmentData>();
    deserializationRegistry.registerClass<SelectOptionalsModifierData>();
    deserializationRegistry.registerClass<SelectRecordVariantModifierData>();
    deserializationRegistry.registerClass<SetTypeVariableModifierData>();
    deserializationRegistry.registerClass<IntValue>();
    deserializationRegistry.registerClass<StringValue>();
    deserializationRegistry.registerClass<RationalValue>();
    deserializationRegistry.registerClass<EnumValue>();
    deserializationRegistry.registerClass<TupleValue>();
    deserializationRegistry.registerClass<MapValue>();
    deserializationRegistry.registerClass<FieldIdValue>();
    deserializationRegistry.registerClass<OneToOneMapEntryData>();
    deserializationRegistry.registerClass<AllToOneFallbackMapEntryData>();
    deserializationRegistry.registerClass<AllToSameFallbackMapEntryData>();
    deserializationRegistry.registerClass<MapBundle>();
    deserializationRegistry.registerClass<MapProjectDataLocation>();
}
