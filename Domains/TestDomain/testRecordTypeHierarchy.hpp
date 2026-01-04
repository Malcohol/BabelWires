#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

namespace testDomain {

    struct RecordWithNoFields : babelwires::RecordType {
        REGISTERED_TYPE("NoFields", "Record with no fields", "d5d8e6a4-97f4-4aab-a6a0-35a4704ecda2", 1);
        RecordWithNoFields(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordA0 : babelwires::RecordType {
        REGISTERED_TYPE("RecordA0", "Record with field A", "19d7acd1-9097-40fa-866b-94256ef7382c", 1);
        RecordA0(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordA1 : babelwires::RecordType {
        REGISTERED_TYPE("RecordA1", "Different record with field A", "b648e0ed-1911-4daf-9338-b2f449851f0f", 1);
        RecordA1(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordB : babelwires::RecordType {
        REGISTERED_TYPE("RecordB", "Record with int field B", "e5a1e186-60f9-448e-bd75-b9c7e8d6c1ce", 1);
        RecordB(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordAB : babelwires::RecordType {
        REGISTERED_TYPE("RecordAB", "Record with int fields A and B", "35090f6e-5c3e-45b8-94dc-4be3392290a7", 1);
        RecordAB(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordAS : babelwires::RecordType {
        REGISTERED_TYPE("RecordAS", "Record with string field A", "f8ff70ac-a175-447e-89e2-15375d9418ac", 1);
        RecordAS(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordAOpt : babelwires::RecordType {
        REGISTERED_TYPE("RecordAOpt", "Record with int fields A and Opt", "d7cecf7f-0f95-4120-808b-86bd99e2f504", 1);
        RecordAOpt(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordAOptFixed : babelwires::RecordType {
        REGISTERED_TYPE("RecordAOpt", "Record with int fields A and Opt, where in this case, Opt is a fixed field", "a3665c18-e5f2-41b7-8cf0-bf3a1396e029", 1);
        RecordAOptFixed(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordABOpt : babelwires::RecordType {
        REGISTERED_TYPE("RecordABOpt", "Record with int fields A, B and Opt", "5a21780a-0a56-481a-a850-4afb18b3bc2d", 1);
        RecordABOpt(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordAOptS : babelwires::RecordType {
        REGISTERED_TYPE("RecordAOptS", "Record with int field A and string field Opt", "62fd3ee0-3a98-4775-b6fc-2c37143491f6", 1);
        RecordAOptS(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordABOptChild : babelwires::RecordType {
        RecordABOptChild(const babelwires::TypeSystem& typeSystem);
        REGISTERED_TYPE("RecordChild", "Child of RecordABOpt", "0cc46b96-9ce0-4722-aba9-9009d12f1bcc", 1);
    };

    struct RecordAsub0 : babelwires::RecordType {
        REGISTERED_TYPE("RecordAsub0", "Record with field A which is a subtype of the original A", "a2a71062-213e-456e-91dd-0df1ca1a1dae", 1);
        RecordAsub0(const babelwires::TypeSystem& typeSystem);
    };

    struct RecordAsubBsup : babelwires::RecordType {
        REGISTERED_TYPE("RecordAsubBsup", "Record with field A and B, which is a supertype of the original B", "21418e19-f5d6-4f26-9948-e9497595c11d", 1);
        RecordAsubBsup(const babelwires::TypeSystem& typeSystem);
    };
    

} // namespace testDomain