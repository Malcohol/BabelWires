#include <gtest/gtest.h>

#include <Common/Identifiers/identifierRegistry.hpp>

/// The base BabelWiresLib offers some standard functionality using registered identifiers.
/// These only work in the IdentifierRegistry they were registered in, so that cannot be
/// local to the tests.
///
/// On the other hand, there are many things we'd only like to test in the context of a local
/// identifierRegistry, which is easiest to ensure by not having a global one.
/// 
/// This is why there are two versions of the BabelWiresLib tests.
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Since these tests load "real" types which use the REGISTERED_ID macros, they have to work within the same registry singleton.
    babelwires::IdentifierRegistryScope identifierRegistry;

    return RUN_ALL_TESTS();
}
