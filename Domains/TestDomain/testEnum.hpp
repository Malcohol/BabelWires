#include <BabelWiresLib/Types/Enum/enumWithCppEnum.hpp>
#include <BabelWiresLib/TypeSystem/registeredType.hpp>

#pragma once

namespace babelwires {
    class TypeSystem;
}

#define TESTENUM_VALUES(X)                                                                                            \
    X(Foo, "Foo", "9aa73d97-4f7b-4fb3-b57d-e5be55c4e659")                                                                                              \
    X(Bar, "Bar", "dc65b741-5599-43f1-ab9d-b884132821ef")                                                                                              \
    X(Erm, "Erm", "56932f30-a1e8-46e8-b55a-78080cfbafc5")                                                                                              \
    X(Oom, "Oom", "9fb8e407-2413-4600-83c8-c7ac42fbd365")                                                                                              \
    X(Boo, "Boo", "df6e11db-1351-4fc4-b9c1-3d0afacd5284")

namespace testDomain {

// Foo, Bar, Erm, Oom, Boo
    struct TestEnum : babelwires::EnumType {
        REGISTERED_TYPE("Enum", "Enum", "d5cbc549-0c16-45aa-874b-c81b9bef21f3", 1);
        TestEnum();

        /// Expose a C++ enum which matches the Enum.
        ENUM_DEFINE_CPP_ENUM(TESTENUM_VALUES);
    };

    // Bar, Erm, Oom
    struct TestSubEnum : babelwires::EnumType {
        REGISTERED_TYPE("SubEnum", "SubEnum", "80ba8f33-3851-48b4-a510-52770468d5f6", 1);
        TestSubEnum();
    };

    // Bar, Erm
    struct TestSubSubEnum1 : babelwires::EnumType {
        REGISTERED_TYPE("SubSubEnum1", "SubSubEnum1", "5cdf92b9-6d0e-4477-b853-336dafc2828d", 1);
        TestSubSubEnum1();
    };

    // Erm, Oom
    struct TestSubSubEnum2 : babelwires::EnumType {
        REGISTERED_TYPE("SubSubEnum2", "SubSubEnum2", "e787e345-0138-4b46-9f43-d12ed57d4ee9", 1);
        TestSubSubEnum2();
    };
} // namespace testDomain
