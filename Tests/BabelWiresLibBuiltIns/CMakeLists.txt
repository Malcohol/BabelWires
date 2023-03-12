SET( LIB_TESTS_SRCS
    babelWiresBuiltInsTests.cpp
    addBlankTest.cpp
   )

ADD_EXECUTABLE( babelWiresBuiltInsTests ${LIB_TESTS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( babelWiresBuiltInsTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
TARGET_LINK_LIBRARIES(babelWiresBuiltInsTests Common testUtils BabelWiresLib libTestUtils gtest)
