#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/RecordWithVariants/recordWithVariantsType.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

namespace testDomain {

    struct RecordVWithNoFields : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("VarNoFields", "RecordWithVariants with no fields", "20cc79e4-d93b-4c88-a0ff-36b13d415092", 1);
        RecordVWithNoFields();
    };

    struct RecordVA0 : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVA0", "RecordWithVariants with field A", "5bbc0266-4223-43e3-b1be-6fe44ee921bc", 1);
        RecordVA0();
    };

    struct RecordVA1 : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVA1", "Different RecordWithVariants with field A", "a58a442f-ec25-4b71-99f6-d26478869368", 1);
        RecordVA1();
    };

    struct RecordVB : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVB", "RecordWithVariants with field B", "64c451fc-6cdf-4b11-9e81-bf9f6a166aba", 1);
        RecordVB();
    };

    struct RecordVAB : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVAB", "RecordWithVariants with fields A and B", "277df5de-5ae4-49b6-b0d0-cb161a2f6133", 1);
        RecordVAB();
    };

    struct RecordVAS : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVAS", "RecordWithVariants with string field A", "c002ae48-d33e-43d9-ab71-466128e02f9b", 1);
        RecordVAS();
    };

    struct RecordVAV0 : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVAV0", "RecordWithVariants with fields A and 0", "c7ec2e91-a621-4da9-aadb-196b0e566cab", 1);
        RecordVAV0();
    };

    struct RecordVABV0 : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVABV0", "RecordWithVariants with fields A, 0 and B", "b6c11598-6393-45d8-939e-45a3cbf5f0d7", 1);
        RecordVABV0();
    };


    struct RecordVABV1 : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVABV1", "RecordWithVariants with fields A, 1 and B", "77756b6b-0388-4359-a103-36eeefe023f6", 1);
        RecordVABV1();
    };

    struct RecordVABV01 : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVABV01", "RecordWithVariants with fields A, 0, B and 1", "65c4a33e-c88c-4c61-bc9d-0dd2b2f7b021", 1);
        RecordVABV01();
    };

    struct RecordVAVB : babelwires::RecordWithVariantsType {
        PRIMITIVE_TYPE("RecordVAVB", "RecordWithVariants with fields A, and B in default", "0d4e6599-a15b-41eb-aa8a-8d6c51db77b7", 1);
        RecordVAVB();
    };

} // namespace testDomain