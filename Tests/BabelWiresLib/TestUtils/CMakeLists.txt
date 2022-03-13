SET( LIB_TEST_UTILS_SRCS
    testEnum.cpp
    testFeatureElement.cpp
    testFileFormats.cpp
    testProcessor.cpp
    testProjectContext.cpp
    testProjectData.cpp
    testRecord.cpp
    testFeatureWithOptionals.cpp
    testRootFeature.cpp
   )

ADD_LIBRARY( libTestUtils ${LIB_TEST_UTILS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( libTestUtils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. )
TARGET_LINK_LIBRARIES(libTestUtils Common testUtils BabelWiresLib gtest)
