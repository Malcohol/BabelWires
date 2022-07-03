#include <gtest/gtest.h>

#include <BabelWiresLib/Enums/enumWithCppEnum.hpp>
#include <BabelWiresLib/Features/enumFeature.hpp>

#pragma once

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
        TestEnum();

        /// Expose a C++ enum which matches the Enum.
        ENUM_DEFINE_CPP_ENUM(TESTENUM_VALUES);

        static babelwires::LongIdentifier getThisIdentifier();
    };

    // Bar, Erm, Oom
    struct TestSubEnum : babelwires::Enum {
        TestSubEnum();

        static babelwires::LongIdentifier getThisIdentifier();
    };

    // Bar, Erm
    struct TestSubSubEnum1 : babelwires::Enum {
        TestSubSubEnum1();

        static babelwires::LongIdentifier getThisIdentifier();
    };

    // Erm, Oom
    struct TestSubSubEnum2 : babelwires::Enum {
        TestSubSubEnum2();

        static babelwires::LongIdentifier getThisIdentifier();
    };
} // namespace testUtils
