#include <gtest/gtest.h>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Features/enumFeature.hpp>

#pragma once

namespace testUtils {
    /// Get values for the test enum.
    /// Note: The identifiers are not actually registered in the identifierRegistry.
    extern const babelwires::Enum::EnumValues g_enumValues;
    
    // Foo, Bar, Erm, Oom, Boo
    struct TestEnum : babelwires::Enum {
        TestEnum();

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
} // namespace
