#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>

namespace testDomain {
    class TestTupleType : public babelwires::TupleType {
      public:
        REGISTERED_TYPE("TestTupleType", "TestTupleType", "48585b19-90bc-4282-b5d0-dc7987ad5310", 1);
        TestTupleType(const babelwires::TypeSystem& typeSystem)
          : babelwires::TupleType(
              typeSystem,
              {babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()}) {}
    };
} // namespace testDomain
