SET( ALSA_TESTS_SRCS
    alsaTests.cpp
    alsaTest.cpp
   )

ADD_EXECUTABLE( alsaTests ${ALSA_TESTS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( alsaTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../.. )
TARGET_LINK_LIBRARIES(alsaTests Common BabelWires_Alsa testUtils gtest)
