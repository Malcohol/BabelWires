#include <BabelWiresLib/Project/FeatureElements/ValueElement/valueElementData.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>
#include <BabelWiresLib/Types/Array/arrayType.hpp>

namespace testUtils {
    class TestSimpleArrayType : public babelwires::ArrayType {
      public:
        TestSimpleArrayType();

        static constexpr unsigned int s_minimumSize = 0;
        static constexpr unsigned int s_maximumSize = 10;
        static constexpr unsigned int s_defaultSize = 4;
        static constexpr unsigned int s_nonDefaultSize = 6;

        static babelwires::TypeRef getExpectedEntryType();

        PRIMITIVE_TYPE("sarray", "Test Simple Array", "2bca3f50-8d9d-46ba-bf43-e0a583f5bcf7", 1);
    };

    class TestCompoundArrayType : public babelwires::ArrayType {
      public:
        TestCompoundArrayType();

        static constexpr unsigned int s_minimumSize = 2;
        static constexpr unsigned int s_maximumSize = 4;
        static constexpr unsigned int s_defaultSize = 3;

        static babelwires::TypeRef getExpectedEntryType();

        PRIMITIVE_TYPE("carray", "Test Compound Array", "e2125eb9-b8a5-470a-a287-7142ff5a6b9c", 1);
    };

    /// ValueElementData which creates an element carrying a simple array.
    class TestArrayElementData : public babelwires::ValueElementData {
      public:
        TestArrayElementData();

        static babelwires::FeaturePath getPathToArray();
        static babelwires::FeaturePath getPathToArrayEntry(unsigned int i);
        static babelwires::FeaturePath getPathToArray_1();
        static babelwires::FeaturePath getPathToArray_2();
    };

} // namespace testUtils
