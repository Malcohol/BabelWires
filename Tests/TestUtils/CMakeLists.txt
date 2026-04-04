SET( TEST_UTILS_SRCS
   testLog.cpp
   tempFilePath.cpp
   testIdentifiers.cpp
   )

ADD_LIBRARY( testUtils ${TEST_UTILS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( testUtils PRIVATE ${PROJECT_SOURCE_DIR} )
TARGET_LINK_LIBRARIES(testUtils BaseLib)
