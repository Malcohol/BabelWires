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
TARGET_INCLUDE_DIRECTORIES( libTestDomain PRIVATE ${PROJECT_SOURCE_DIR} )
TARGET_LINK_LIBRARIES(libTestDomain BaseLib BabelWiresLib)

if(BUILD_SHARED_LIBS)
    set_target_properties(libTestDomain PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
    target_compile_definitions(libTestDomain PRIVATE TESTDOMAIN_EXPORTS)
else()
    target_compile_definitions(libTestDomain PUBLIC TESTDOMAIN_STATIC)
endif()
