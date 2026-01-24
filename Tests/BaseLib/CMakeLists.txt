SET( COMMON_TESTS_SRCS
   blockStreamTest.cpp
   cloneTest.cpp
   commonTests.cpp
   dataBundleTest.cpp
   dataSourceTest.cpp
   filePathTest.cpp
   enumFlagsTest.cpp
   exceptionTest.cpp
   hashUtilTest.cpp
   identifierTest.cpp
   logTest.cpp
   multiKeyMapTest.cpp
   outFileStreamTest.cpp
   pointerRangeTest.cpp
   rationalTest.cpp
   registryTest.cpp
   resultTest.cpp
   serializationTest.cpp
   signalTest.cpp
   streamEventHolderTest.cpp
   commonTest.cpp
   uuidTest.cpp
   )

ADD_EXECUTABLE( commonTests ${COMMON_TESTS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( commonTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
TARGET_LINK_LIBRARIES(commonTests BaseLib testUtils gtest)
