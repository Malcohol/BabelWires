#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace testUtils {
  
    class TestRecordWithVariantsType : public babelwires::RecordWithVariantsType {
      public:
        TestRecordWithVariantsType();
        
        PRIMITIVE_TYPE("vrecordT", "RecordWithVariants", "45442c6b-fa02-4a48-b7f8-9ca062c568ea", 1);

        static babelwires::ShortId getTagAId();
        static babelwires::ShortId getTagBId(); // The default tag.
        static babelwires::ShortId getTagCId();
        static babelwires::ShortId getTagDId(); // Identical in effect to A.

        static babelwires::ShortId getFieldA0Id();
        static babelwires::ShortId getFf0Id();
        // This is a TestSimpleRecordType
        static babelwires::ShortId getFieldB0Id();
        static babelwires::ShortId getFieldABId();
        // This is a TestSimpleRecordType
        static babelwires::ShortId getFieldA1Id();
        static babelwires::ShortId getFf1Id();
        static babelwires::ShortId getFieldBCId();
    };
}
