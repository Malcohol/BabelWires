SET( TEST_UTILS_SRCS
   testLog.cpp
   tempFilePath.cpp
   testIdentifiers.cpp
   testStrings.cpp
   )

ADD_LIBRARY( testUtils STATIC ${TEST_UTILS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( testUtils PRIVATE ${PROJECT_SOURCE_DIR} )
TARGET_LINK_LIBRARIES(testUtils BaseLib)
