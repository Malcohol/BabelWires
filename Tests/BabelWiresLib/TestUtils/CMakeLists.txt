SET( LIB_TEST_UTILS_SRCS
    testNode.cpp
    testEnvironment.cpp
    testProjectData.cpp
    testProjectDataWithCompoundConnection.cpp
    testValueAndType.cpp
    testTypeConstructor.cpp
   )

ADD_LIBRARY( libTestUtils ${LIB_TEST_UTILS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( libTestUtils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. )
TARGET_LINK_LIBRARIES(libTestUtils BaseLib testUtils libTestDomain BabelWiresLib gtest)
