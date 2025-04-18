#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace testDomain {
    class TestSumType : public babelwires::SumType {
      public:
        PRIMITIVE_TYPE("TestSumType", "TestSumType", "19c1e116-5e37-489e-9205-8d0b0a023f13", 1);
        TestSumType(unsigned int defaultType = 1)
            : babelwires::SumType({babelwires::DefaultIntType::getThisType(),
                                   babelwires::DefaultRationalType::getThisType()}, defaultType) {}
    };
} // namespace
