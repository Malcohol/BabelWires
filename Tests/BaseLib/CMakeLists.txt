SET( COMMON_TESTS_SRCS
   buildCompatibilityTest.cpp
   pluginSupportTest.cpp
   blockStreamTest.cpp
   cloneTest.cpp
   contextTest.cpp
   baseLibTests.cpp
   dataBundleTest.cpp
   dataSourceTest.cpp
   downcastableTest.cpp
   fileDataSinkTest.cpp
   filePathTest.cpp
   enumFlagsTest.cpp
   hashUtilTest.cpp
   identifierTest.cpp
   logTest.cpp
   multiKeyMapTest.cpp
   pointerRangeTest.cpp
   queryableInterfacesTest.cpp
   rationalTest.cpp
   randomServiceTest.cpp
   registryTest.cpp
   resultTest.cpp
   serializationTest.cpp
   xmlSerializationTest.cpp
   yamlSerializationTest.cpp
   signalTest.cpp
   streamEventHolderTest.cpp
   commonTest.cpp
   versionTest.cpp
   uuidTest.cpp
   )

set(BASELIB_TEST_PLUGIN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/plugins")

add_subdirectory(pluginTestModules)

ADD_EXECUTABLE( baseLibTests ${COMMON_TESTS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( baseLibTests PRIVATE ${PROJECT_SOURCE_DIR} )
TARGET_LINK_LIBRARIES(baseLibTests BaseLib testUtils gtest)
target_compile_definitions(baseLibTests PRIVATE
   BASELIB_TEST_PLUGIN_OUTPUT_DIR="$<TARGET_FILE_DIR:baseLibTestPluginGood>"
   BASELIB_TEST_PLUGIN_GOOD_PATH="$<TARGET_FILE:baseLibTestPluginGood>"
   BASELIB_TEST_PLUGIN_BAD_MAGIC_PATH="$<TARGET_FILE:baseLibTestPluginBadMagic>"
   BASELIB_TEST_PLUGIN_FINGERPRINT_MISMATCH_PATH="$<TARGET_FILE:baseLibTestPluginFingerprintMismatch>"
   BASELIB_TEST_PLUGIN_MISSING_PROBE_PATH="$<TARGET_FILE:baseLibTestPluginMissingProbe>"
   BASELIB_TEST_PLUGIN_MISSING_REGISTRATION_PATH="$<TARGET_FILE:baseLibTestPluginMissingRegistration>"
)

if(BUILD_TESTING)
   register_test_executable(baseLibTests)
endif()
