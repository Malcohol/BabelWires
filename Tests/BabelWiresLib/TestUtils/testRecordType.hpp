#pragma once

#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BabelWiresLib/Instance/instance.hpp>
#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace testUtils {

    class TestSimpleRecordType : public babelwires::RecordType {
      public:
        TestSimpleRecordType();

        PRIMITIVE_TYPE("srecordT", "SimpleRecord", "ea96a409-6424-4924-aefe-ecbe66139f17", 1);

        DECLARE_INSTANCE_BEGIN(TestSimpleRecordType)
        DECLARE_INSTANCE_FIELD(intR0, babelwires::IntType)
        DECLARE_INSTANCE_FIELD(intR1, babelwires::IntType)
        DECLARE_INSTANCE_END()

        // Specifically for unit testing.

        static babelwires::ShortId getInt0Id();
        static babelwires::ShortId getInt1Id();

        static constexpr char s_int0IdInitializer[] = "intR0";
        static constexpr char s_int1IdInitializer[] = "intR1";

        static constexpr char s_int0FieldName[] = "Int0";
        static constexpr char s_int1FieldName[] = "Int1";

        static constexpr char s_int0Uuid[] = "00000000-1111-2222-3333-800070000001";
        static constexpr char s_int1Uuid[] = "00000000-1111-2222-3333-800070000002";
    };

    class TestComplexRecordType : public babelwires::RecordType {
      public:
        TestComplexRecordType();

        PRIMITIVE_TYPE("crecordT", "ComplexRecord", "87291871-677d-41a1-81e7-bf1206b1d396", 1);

        static constexpr int c_int1min = -10;
        static constexpr int c_int1max = 10;
        static constexpr int c_int1default = 2;

        static constexpr char s_intIdInitializer[] = "intR0";
        static constexpr char s_opIntIdInitializer[] = "opInt";
        static constexpr char s_subRecordIdInitializer[] = "rec";
        static constexpr char s_int1IdInitializer[] = "intR1";
        static constexpr char s_opRecIdInitializer[] = "opRec";
        static constexpr char s_stringIdInitializer[] = "string";
        static constexpr char s_arrayIdInitializer[] = "array";

        static constexpr char s_intFieldName[] = "Int0";
        static constexpr char s_opIntFieldName[] = "OptionalInt";
        static constexpr char s_subRecordFieldName[] = "Subrecord";
        static constexpr char s_int1FieldName[] = "Int1";
        static constexpr char s_opRecFieldName[] = "OptionalRecord";
        static constexpr char s_stringFieldName[] = "String";
        static constexpr char s_arrayFieldName[] = "Array";

        static babelwires::ShortId getInt0Id();
        static babelwires::ShortId getOpIntId();
        static babelwires::ShortId getSubrecordId();
        // Has a range from -10 to 10 with a default of 2.
        static babelwires::ShortId getInt1Id();
        static babelwires::ShortId getOpRecId();
        static babelwires::ShortId getStringId();
        static babelwires::ShortId getArrayId();

        static constexpr unsigned int s_numNonOptionalFields = 5;

        DECLARE_INSTANCE_BEGIN(TestComplexRecordType)
        DECLARE_INSTANCE_FIELD(intR0, babelwires::IntType)
        DECLARE_INSTANCE_FIELD_OPTIONAL(opInt, babelwires::IntType)
        DECLARE_INSTANCE_FIELD(rec, TestSimpleRecordType)
        DECLARE_INSTANCE_FIELD(intR1, babelwires::IntType)
        DECLARE_INSTANCE_FIELD(opRec, TestSimpleRecordType)
        DECLARE_INSTANCE_FIELD(string, babelwires::StringType)
        DECLARE_INSTANCE_ARRAY_FIELD(array, babelwires::IntType)
        DECLARE_INSTANCE_END()
    };

    /// ValueElementData which creates an element carrying a simple record type.
    class TestSimpleRecordElementData : public babelwires::ValueElementData {
      public:
        TestSimpleRecordElementData();

        static babelwires::FeaturePath getPathToRecord();
        static babelwires::FeaturePath getPathToRecordInt0();
    };

    class TestComplexRecordElementData : public babelwires::ValueElementData {
      public:
        TestComplexRecordElementData();

        static babelwires::FeaturePath getPathToRecord();
        static babelwires::FeaturePath getPathToRecordInt0();
        static babelwires::FeaturePath getPathToRecordOpInt();
        static babelwires::FeaturePath getPathToRecordSubrecord();
        static babelwires::FeaturePath getPathToRecordSubrecordInt1();
        static babelwires::FeaturePath getPathToRecordInt1();
        static babelwires::FeaturePath getPathToRecordOpRec();
        static babelwires::FeaturePath getPathToRecordString();
        static babelwires::FeaturePath getPathToRecordArray();
        static babelwires::FeaturePath getPathToRecordArrayEntry(unsigned int i);
    };


    /// Useful for unit tests which want to access parts of the feature hierarchy of this record type
    /// without using the Instance system.
    // Currently not all features are represented.
    struct TestComplexRecordTypeFeatureInfo {
        const babelwires::Feature& m_record;
        const babelwires::Feature& m_intFeature;
        const babelwires::Feature& m_subRecordFeature;
        const babelwires::Feature& m_subRecordIntFeature;
        const babelwires::Feature& m_arrayFeature;
        const babelwires::Feature& m_elem0;
        const babelwires::Feature& m_elem1;

        // These paths are obtained from the features (which tests some FeaturePath functionality).
        babelwires::FeaturePath m_pathToRecord;
        babelwires::FeaturePath m_pathToSubRecord;
        babelwires::FeaturePath m_pathToSubRecordInt;
        babelwires::FeaturePath m_pathToInt;
        babelwires::FeaturePath m_pathToArray;
        babelwires::FeaturePath m_pathToElem0;
        babelwires::FeaturePath m_pathToElem1;

        TestComplexRecordTypeFeatureInfo(const babelwires::Feature& testRecord);
    };
} // namespace testUtils
