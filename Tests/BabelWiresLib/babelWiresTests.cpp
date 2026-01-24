#include <gtest/gtest.h>

#include <BaseLib/Identifiers/identifierRegistry.hpp>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // The base BabelWiresLib offers some standard functionality using registered identifiers.
    // These only work in the IdentifierRegistry they were registered in, so that cannot be
    // local to the tests. Thus this same identifier registry needs to be set.
    babelwires::IdentifierRegistryScope identifierRegistry;

    return RUN_ALL_TESTS();
}
