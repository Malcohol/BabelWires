SET( COMMON_TESTS_SRCS
   buildCompatibilityTest.cpp
   blockStreamTest.cpp
   cloneTest.cpp
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
   rationalTest.cpp
   registryTest.cpp
   resultTest.cpp
   serializationTest.cpp
   signalTest.cpp
   streamEventHolderTest.cpp
   commonTest.cpp
   versionTest.cpp
   uuidTest.cpp
   )

ADD_EXECUTABLE( baseLibTests ${COMMON_TESTS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( baseLibTests PRIVATE ${PROJECT_SOURCE_DIR} )
TARGET_LINK_LIBRARIES(baseLibTests BaseLib testUtils gtest)

if(BUILD_TESTING)
   gtest_discover_tests(baseLibTests TEST_LIST baseLibDiscoveredTests)
   configure_shared_test_runtime(baseLibDiscoveredTests baseLibTests)
endif()
