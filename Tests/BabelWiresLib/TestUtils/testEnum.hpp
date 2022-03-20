#include <gtest/gtest.h>

#include <BabelWiresLib/Enums/enum.hpp>
#include <BabelWiresLib/Features/enumFeature.hpp>

namespace testUtils {
    /// Get values for the test enum.
    /// Note: The identifiers are not registered.
    babelwires::Enum::EnumValues getTestEnumValues();
    struct TestEnum : babelwires::Enum {
        TestEnum();

        static babelwires::LongIdentifier getThisIdentifier();
    };
} // namespace
