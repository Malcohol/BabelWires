SET( LIB_TEST_UTILS_SRCS
    libRegistration.cpp
    testArrayType.cpp
    testEnum.cpp
    testFileFormats.cpp
    testGenericType.cpp
    testParallelProcessor.cpp
    testProcessor.cpp
    testRecordType.cpp
    testRecordTypeHierarchy.cpp
    testRecordWithVariantsType.cpp
    testRecordWithVariantsTypeHierarchy.cpp
   )

ADD_LIBRARY( libTestDomain ${LIB_TEST_UTILS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( libTestDomain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
TARGET_LINK_LIBRARIES(libTestDomain BaseLib BabelWiresLib)
