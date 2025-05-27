#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Int/intType.hpp>
#include <BabelWiresLib/Types/Int/intTypeConstructor.hpp>
#include <BabelWiresLib/Types/Rational/rationalType.hpp>
#include <BabelWiresLib/Types/Rational/rationalTypeConstructor.hpp>
#include <BabelWiresLib/Types/String/stringType.hpp>
#include <BabelWiresLib/Types/Sum/sumType.hpp>

namespace testDomain {
    class TestSumType : public babelwires::SumType {
      public:
        PRIMITIVE_TYPE("TestSumType", "TestSumType", "19c1e116-5e37-489e-9205-8d0b0a023f13", 1);
        TestSumType(unsigned int defaultType = 1)
            : babelwires::SumType(
                  {babelwires::DefaultIntType::getThisType(), babelwires::DefaultRationalType::getThisType()},
                  defaultType) {}
    };

    class TestSumTypeZnQn : public babelwires::SumType {
      public:
        PRIMITIVE_TYPE("TestSumZnQn", "TestSumTypeZnQn", "7fe319f2-ef9b-4edf-aeb6-313d1036c239", 1);
        TestSumTypeZnQn()
            : babelwires::SumType(
                  {babelwires::IntTypeConstructor::makeTypeRef(0, 4), babelwires::RationalTypeConstructor::makeTypeRef(0, 4)}) {}
    };

    class TestSumTypeZwQn : public babelwires::SumType {
      public:
        PRIMITIVE_TYPE("TestSumZwQn", "TestSumTypeZwQn", "012f3498-ff27-40ea-89b1-21f5599e895f", 1);
        TestSumTypeZwQn()
            : babelwires::SumType(
                  {babelwires::IntTypeConstructor::makeTypeRef(0, 16), babelwires::RationalTypeConstructor::makeTypeRef(0, 4)}) {}
    };

    class TestSumTypeZwQw : public babelwires::SumType {
      public:
        PRIMITIVE_TYPE("TestSumZwQw", "TestSumTypeZwQw", "3fe2e1c0-45cb-4253-93b1-42b5b7b131be", 1);
        TestSumTypeZwQw()
            : babelwires::SumType(
                  {babelwires::IntTypeConstructor::makeTypeRef(0, 16), babelwires::RationalTypeConstructor::makeTypeRef(0, 16)}) {}
    };

    class TestSumTypeZnQwS : public babelwires::SumType {
      public:
        PRIMITIVE_TYPE("TestSumZnQwS", "TestSumTypeZnQwS", "96e6be74-61e2-4366-8e36-abb716322c6e", 1);
        TestSumTypeZnQwS()
            : babelwires::SumType(
                  {babelwires::IntTypeConstructor::makeTypeRef(0, 4), babelwires::RationalTypeConstructor::makeTypeRef(0, 16), babelwires::StringType::getThisType()}) {}
    };

  } // namespace testDomain
