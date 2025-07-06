#include <BabelWiresLib/Types/Generic/genericType.hpp>

#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

namespace testDomain {

    class TestGenericType : public babelwires::GenericType {
      public:
        PRIMITIVE_TYPE("TGen", "Test Generic Type", "80013367-d279-4f9d-aa38-6c397b31d38d", 1);
    };
} // namespace testDomain
