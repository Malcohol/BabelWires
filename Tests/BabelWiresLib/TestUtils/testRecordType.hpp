#pragma once

#include <BabelWiresLib/Types/Record/recordType.hpp>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

#include <BabelWiresLib/Instance/instance.hpp>


namespace testUtils {
    
    class TestSimpleRecordType : public babelwires::RecordType {
      public:
        TestSimpleRecordType();
        
        PRIMITIVE_TYPE("srecordT", "SimpleRecord", "ea96a409-6424-4924-aefe-ecbe66139f17", 1);

        static babelwires::ShortId getInt0Id();
        static babelwires::ShortId getInt1Id();

        DECLARE_INSTANCE_BEGIN(TestSimpleRecordType)
        DECLARE_INSTANCE_FIELD(intR0, babelwires::IntType)
        DECLARE_INSTANCE_FIELD(intR1, babelwires::IntType)
        DECLARE_INSTANCE_END()
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
