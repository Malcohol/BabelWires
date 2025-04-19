#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>

namespace testDomain {

    struct RecordWithNoFields : babelwires::RecordType {
        PRIMITIVE_TYPE("NoFields", "Record with no fields", "d5d8e6a4-97f4-4aab-a6a0-35a4704ecda2", 1);
        RecordWithNoFields();
    };

    struct RecordA0 : babelwires::RecordType {
        PRIMITIVE_TYPE("RecordA0", "Record with field A", "19d7acd1-9097-40fa-866b-94256ef7382c", 1);
        RecordA0();
    };

    struct RecordA1 : babelwires::RecordType {
        PRIMITIVE_TYPE("RecordA1", "Different record with field A", "b648e0ed-1911-4daf-9338-b2f449851f0f", 1);
        RecordA1();
    };

    struct RecordB : babelwires::RecordType {
        PRIMITIVE_TYPE("RecordB", "Record with int field B", "e5a1e186-60f9-448e-bd75-b9c7e8d6c1ce", 1);
        RecordB();
    };

    struct RecordAB : babelwires::RecordType {
        PRIMITIVE_TYPE("RecordAB", "Record with int fields A and B", "35090f6e-5c3e-45b8-94dc-4be3392290a7", 1);
        RecordAB();
    };

    struct RecordAS : babelwires::RecordType {
        PRIMITIVE_TYPE("RecordAS", "Record with string field A", "f8ff70ac-a175-447e-89e2-15375d9418ac", 1);
        RecordAS();
    };

    struct RecordAOpt : babelwires::RecordType {
        PRIMITIVE_TYPE("RecordAOpt", "Record with int fields A and Opt", "d7cecf7f-0f95-4120-808b-86bd99e2f504", 1);
        RecordAOpt();
    };

    struct RecordABOpt : babelwires::RecordType {
        PRIMITIVE_TYPE("RecordABOpt", "Record with int fields A, B and Opt", "5a21780a-0a56-481a-a850-4afb18b3bc2d", 1);
        RecordABOpt();
    };

    struct RecordAOptS : babelwires::RecordType {
        PRIMITIVE_TYPE("RecordAOptS", "Record with int field A and string field Opt", "62fd3ee0-3a98-4775-b6fc-2c37143491f6", 1);
        RecordAOptS();
    };

    struct RecordABOptChild : babelwires::RecordType {
        RecordABOptChild(const babelwires::TypeSystem& typeSystem);
        PRIMITIVE_TYPE("RecordChild", "Child of RecordABOpt", "0cc46b96-9ce0-4722-aba9-9009d12f1bcc", 1);
    };

} // namespace testDomain