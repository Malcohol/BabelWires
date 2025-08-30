#include <BabelWiresLib/Types/Generic/genericType.hpp>

#include <BabelWiresLib/TypeSystem/registeredType.hpp>
#include <BabelWiresLib/Path/path.hpp>

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

        static babelwires::Path getPathToArray();

        static babelwires::Path getPathToVar0_member();
        static babelwires::Path getPathToVar0_inNestedGenericType();
        static babelwires::Path getPathToNestedVar0();
        static babelwires::Path getPathToVar0_inArray();

        static babelwires::Path getPathToVar1_member();
    };
} // namespace testDomain
