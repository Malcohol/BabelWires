#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Tuple/tupleType.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

namespace testUtils {
    class TestTupleType : public babelwires::TupleType {
      public:
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(testUtils::getTestRegisteredMediumIdentifier("TestTupleType"), 1);
        TestTupleType()
            : babelwires::TupleType({babelwires::DefaultIntType::getThisType(),
                                   babelwires::DefaultRationalType::getThisType()}) {}
    };
} // namespace
