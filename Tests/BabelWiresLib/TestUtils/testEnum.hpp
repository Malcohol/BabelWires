#include <gtest/gtest.h>

#include <BabelWiresLib/Types/Enum/enumWithCppEnum.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#pragma once

namespace babelwires {
    class TypeSystem;
}

/// The TestEnum uses some of the enumWithCppEnum macros. It doesn't use the full set, since they register identifiers
/// using in standard ways (using magic statics) which don't work in the the Common and BabelWires tests libs.
#define TESTENUM_VALUES(X)                                                                                            \
    X(Foo, "Foo", "9aa73d97-4f7b-4fb3-b57d-e5be55c4e659")                                                                                              \
    X(Bar, "Bar", "dc65b741-5599-43f1-ab9d-b884132821ef")                                                                                              \
    X(Erm, "Erm", "56932f30-a1e8-46e8-b55a-78080cfbafc5")                                                                                              \
    X(Oom, "Oom", "9fb8e407-2413-4600-83c8-c7ac42fbd365")                                                                                              \
    X(Boo, "Boo", "df6e11db-1351-4fc4-b9c1-3d0afacd5284")

namespace testUtils {
    /// Get values for the test enum.
    /// Note: The identifiers are not actually registered in the identifierRegistry.
    extern const babelwires::EnumType::ValueSet g_enumValues;

// Foo, Bar, Erm, Oom, Boo
    struct TestEnum : babelwires::EnumType {
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("Enum"), 1);
        TestEnum();

        /// Expose a C++ enum which matches the Enum.
        ENUM_DEFINE_CPP_ENUM(TESTENUM_VALUES);
    };

    // Bar, Erm, Oom
    struct TestSubEnum : babelwires::EnumType {
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("SubEnum"), 1);
        TestSubEnum();
    };

    // Bar, Erm
    struct TestSubSubEnum1 : babelwires::EnumType {
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("SubSubEnum1"), 1);
        TestSubSubEnum1();
    };

    // Erm, Oom
    struct TestSubSubEnum2 : babelwires::EnumType {
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("SubSubEnum2"), 1);
        TestSubSubEnum2();
    };
} // namespace testUtils
