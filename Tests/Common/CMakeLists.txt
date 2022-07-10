SET( COMMON_TESTS_SRCS
   audioInterfaceRegistryTest.cpp
   blockStreamTest.cpp
   cloneTest.cpp
   commonTests.cpp
   dataSourceTest.cpp
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
   typesTest.cpp
   uuidTest.cpp
   )

ADD_EXECUTABLE( commonTests ${COMMON_TESTS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( commonTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
TARGET_LINK_LIBRARIES(commonTests Common testUtils gtest)
