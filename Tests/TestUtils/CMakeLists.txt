SET( TEST_UTILS_SRCS
   testLog.cpp
   tempFilePath.cpp
   testDeserializationRegistry.cpp
   )

ADD_LIBRARY( testUtils ${TEST_UTILS_SRCS} )

TARGET_LINK_LIBRARIES(testUtils Common)
