#include <BabelWiresLib/Types/Generic/genericType.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Types/Record/recordType.hpp>

namespace testDomain {

    class TestGenericType : public babelwires::GenericType {
      public:
        REGISTERED_TYPE("TGen", "Test Generic Type", "80013367-d279-4f9d-aa38-6c397b31d38d", 1);
        TestGenericType();

        static babelwires::ShortId getIdOfX();
        static babelwires::ShortId getIdOfY();
        static babelwires::ShortId getIdOfInt();
        static babelwires::ShortId getIdOfNestedGenericType();
        static babelwires::ShortId getIdOfNestedX();
        static babelwires::ShortId getIdOfNestedZ();
        static babelwires::ShortId getIdOfArray();

        static babelwires::Path getPathToWrappedType();
        static babelwires::Path getPathToX();
        static babelwires::Path getPathToY();
        static babelwires::Path getPathToInt();
        static babelwires::Path getPathToNestedGenericType();
        static babelwires::Path getPathToNestedWrappedType();
        static babelwires::Path getPathToNestedX();
        static babelwires::Path getPathToNestedZ();
        static babelwires::Path getPathToArray();
        static babelwires::Path getPathToArray0();
    };

    /// A simple compound type that can be used to instantiate the record in the nested generic type above.
    class TestSimpleCompoundType : public babelwires::RecordType {
      public:
        TestSimpleCompoundType();

        REGISTERED_TYPE("scompoundT", "SimpleCompound", "a22a400c-66f0-4033-b29a-45af5f334476", 1);
    };

 
} // namespace testDomain
