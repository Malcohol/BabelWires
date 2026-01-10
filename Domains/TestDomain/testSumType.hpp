#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>

namespace testDomain {
    class TestSumType : public babelwires::SumType {
      public:
        REGISTERED_TYPE("TestSumType", "TestSumType", "19c1e116-5e37-489e-9205-8d0b0a023f13", 1);
        TestSumType(const babelwires::TypeSystem& typeSystem, unsigned int defaultType = 1)
            : babelwires::SumType(getThisType(), 
                  typeSystem,
                  {babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()},
                  defaultType) {}
    };
} // namespace testDomain
