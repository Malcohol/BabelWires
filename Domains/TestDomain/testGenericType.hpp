#include <BabelWiresLib/Types/Generic/genericType.hpp>

#include <BabelWiresLib/Path/path.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>

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
} // namespace testDomain
