#include <BabelWiresLib/Types/Array/arrayType.hpp>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace testUtils {
    class TestArrayType : public babelwires::ArrayType {
      public:
        TestArrayType();

        PRIMITIVE_TYPE("tarray", "Test Array", "2bca3f50-8d9d-46ba-bf43-e0a583f5bcf7", 1);
    };

    class TestArrayType2 : public babelwires::ArrayType {
      public:
        TestArrayType2();

        PRIMITIVE_TYPE("tarray2", "Test Array2", "e2125eb9-b8a5-470a-a287-7142ff5a6b9c", 1);
    };
} // namespace testUtils