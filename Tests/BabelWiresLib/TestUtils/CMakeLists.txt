SET( LIB_TEST_UTILS_SRCS
    testArrayType.cpp
    testEnum.cpp
    testNode.cpp
    testFileFormats.cpp
    testProcessor.cpp
    testEnvironment.cpp
    testProjectData.cpp
    testRecordType.cpp
    testRecordWithVariantsType.cpp
    testValueAndType.cpp
    testTypeConstructor.cpp
   )

ADD_LIBRARY( libTestUtils ${LIB_TEST_UTILS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( libTestUtils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. )
TARGET_LINK_LIBRARIES(libTestUtils Common testUtils BabelWiresLib gtest)
