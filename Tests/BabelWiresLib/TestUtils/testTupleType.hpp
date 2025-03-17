#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

namespace testUtils {
    class TestTupleType : public babelwires::TupleType {
      public:
        PRIMITIVE_TYPE("TestTupleType", "TestTupleType", "48585b19-90bc-4282-b5d0-dc7987ad5310", 1);
        TestTupleType()
            : babelwires::TupleType(
                  {babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()}) {}
    };
} // namespace testUtils
