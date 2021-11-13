SET( TEST_UTILS_SRCS
   testLog.cpp
   tempFilePath.cpp
   testDeserializationRegistry.cpp
   testIdentifiers.cpp
   )

ADD_LIBRARY( testUtils ${TEST_UTILS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( testUtils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
TARGET_LINK_LIBRARIES(testUtils Common)
