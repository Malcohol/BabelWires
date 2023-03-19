#include <gtest/gtest.h>

#include <BabelWiresLib/Enums/enumWithCppEnum.hpp>
#include <BabelWiresLib/Enums/enumFeature.hpp>
#include <BabelWiresLib/TypeSystem/primitiveType.hpp>

#include <Tests/TestUtils/testIdentifiers.hpp>

#pragma once

namespace babelwires {
    class TypeSystem;
}

/// The TestEnum uses some of the enumWithCppEnum macros. It doesn't use the full set, since they register identifiers
/// using in standard ways (using magic statics) which don't work in the the Common and BabelWires tests libs.
#define TESTENUM_VALUES(X)                                                                                            \
    X(Foo, "Foo", "UUID")                                                                                              \
    X(Bar, "Bar", "UUID")                                                                                              \
    X(Erm, "Erm", "UUID")                                                                                              \
    X(Oom, "Oom", "UUID")                                                                                              \
    X(Boo, "Boo", "UUID")

namespace testUtils {
    /// Get values for the test enum.
    /// Note: The identifiers are not actually registered in the identifierRegistry.
    extern const babelwires::Enum::EnumValues g_enumValues;

// Foo, Bar, Erm, Oom, Boo
    struct TestEnum : babelwires::Enum {
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("Enum"), 1);
        TestEnum();

        /// Expose a C++ enum which matches the Enum.
        ENUM_DEFINE_CPP_ENUM(TESTENUM_VALUES);
    };

    // Bar, Erm, Oom
    struct TestSubEnum : babelwires::Enum {
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("SubEnum"), 1);
        TestSubEnum();
    };

    // Bar, Erm
    struct TestSubSubEnum1 : babelwires::Enum {
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("SubSubEnum1"), 1);
        TestSubSubEnum1();
    };

    // Erm, Oom
    struct TestSubSubEnum2 : babelwires::Enum {
        PRIMITIVE_TYPE_WITH_REGISTERED_ID(getTestRegisteredMediumIdentifier("SubSubEnum2"), 1);
        TestSubSubEnum2();
    };

    void addTestEnumTypes(babelwires::TypeSystem& typeSystem);
} // namespace testUtils
