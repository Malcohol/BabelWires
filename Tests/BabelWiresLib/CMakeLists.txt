SET( LIB_TESTS_SRCS
    babelWiresTests.cpp
    activateOptionalsModifierDataTest.cpp
    activateOptionalCommandTest.cpp
    addElementCommandTest.cpp
    addEntryToArrayCommandTest.cpp
    addModifierCommandTest.cpp
    changeFileCommandTest.cpp
    commandManagerTest.cpp
    connectionDescriptionTest.cpp
    contentsCacheTest.cpp
    deactivateOptionalCommandTest.cpp
    elementDataTest.cpp
    enumTest.cpp
    featureElementConnectionTest.cpp
    featureElementTest.cpp
    featurePathTest.cpp
    featureTest.cpp
    filePathTest.cpp
    valueNamesTest.cpp
    editTreeTest.cpp
    modifierDataTest.cpp
    modifierTest.cpp
    moveElementCommandTest.cpp
    parallelProcessorTest.cpp
    pasteElementsCommandTest.cpp
    processorElementTest.cpp
    projectBundleTest.cpp
    projectDataTest.cpp
    projectObserverTest.cpp
    projectSerializationTest.cpp
    projectTest.cpp
    recordWithOptionalsFeatureTest.cpp
    removeElementCommandTest.cpp
    removeEntryFromArrayCommandTest.cpp
    removeFailedModifiersCommandTest.cpp
    removeSimpleModifierCommandTest.cpp
    removeModifierCommandTest.cpp
    resizeElementCommandTest.cpp
    setArraySizeCommandTest.cpp
    setExpandedCommandTest.cpp
    sourceFileElementTest.cpp
    targetFileElementTest.cpp
    TestUtils/testEnum.cpp
    TestUtils/testFeatureElement.cpp
    TestUtils/testFileFormats.cpp
    TestUtils/testProcessor.cpp
    TestUtils/testProjectContext.cpp
    TestUtils/testProjectData.cpp
    TestUtils/testRecord.cpp
    TestUtils/testFeatureWithOptionals.cpp
   )

ADD_EXECUTABLE( babelWiresTests ${LIB_TESTS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( babelWiresTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
TARGET_LINK_LIBRARIES(babelWiresTests Common testUtils BabelWiresLib gtest)
