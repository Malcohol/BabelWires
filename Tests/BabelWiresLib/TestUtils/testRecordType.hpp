#pragma once

#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

#include <BabelWiresLib/Instance/instance.hpp>


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

        static babelwires::ShortId getInt0Id();
        static babelwires::ShortId getOpIntId();
        static babelwires::ShortId getSubrecordId();
        static babelwires::ShortId getInt1Id();
        static babelwires::ShortId getOpRecId();
    };

}
