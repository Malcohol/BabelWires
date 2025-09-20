SET( LIB_TESTS_SRCS
    babelWiresTests.cpp
    activateOptionalsModifierDataTest.cpp
    activateOptionalCommandTest.cpp
    addConnectionCommandTest.cpp
    addNodeCommandTest.cpp
    addNodeForInputTreeValueCommandTest.cpp
    addNodeForOutputTreeValueCommandTest.cpp
    addEntryToArrayCommandTest.cpp
    addModifierCommandTest.cpp
    addEntryToMapCommandTest.cpp
    arrayTypeTest.cpp
    changeEntryKindCommandTest.cpp
    changeFileCommandTest.cpp
    commandManagerTest.cpp
    connectionDescriptionTest.cpp
    contentsCacheTest.cpp
    deactivateOptionalCommandTest.cpp
    elementDataTest.cpp
    enumAtomTypeConstructorTest.cpp
    enumTest.cpp
    enumFeatureTest.cpp
    enumUnionTypeConstructorTest.cpp
    nodeConnectionTest.cpp
    nodeTest.cpp
    featurePathTest.cpp
    editTreeTest.cpp
    genericTypeTest.cpp
    intValueTest.cpp
    intTypeTest.cpp
    mapValueTest.cpp
    mapEntryDataTest.cpp
    mapFeatureTest.cpp
    mapHelperTest.cpp
    mapProjectTest.cpp
    modifierDataTest.cpp
    modifierTest.cpp
    moveNodeCommandTest.cpp
    parallelProcessorTest.cpp
    pasteNodesCommandTest.cpp
    pathStepTest.cpp
    processorNodeTest.cpp
    projectBundleTest.cpp
    projectDataTest.cpp
    projectLoadTest.cpp
    projectObserverTest.cpp
    projectSerializationTest.cpp
    projectTest.cpp
    rationalTypeTest.cpp
    rationalValueTest.cpp
    recordTypeTest.cpp
    recordWithVariantsTypeTest.cpp
    removeNodeCommandTest.cpp
    removeEntryFromArrayCommandTest.cpp
    removeEntryFromMapCommandTest.cpp
    removeFailedModifiersCommandTest.cpp
    removeSimpleModifierCommandTest.cpp
    removeModifierCommandTest.cpp
    replaceMapEntryCommandTest.cpp
    resetMapValueCommandTest.cpp
    resizeNodeCommandTest.cpp
    selectRecordVariantModifierDataTest.cpp
    selectRecordVariantCommandTest.cpp
    setArraySizeCommandTest.cpp
    setExpandedCommandTest.cpp
    setMapCommandTest.cpp
    setMapSourceTypeCommandTest.cpp
    setMapTargetTypeCommandTest.cpp
    setTypeVariableCommandTest.cpp
    setTypeVariableModifierDataTest.cpp
    sourceFileNodeTest.cpp
    stringTypeTest.cpp
    stringValueTest.cpp
    subtypeUtilsTest.cpp
    sumOfMapsTypeTest.cpp
    sumTypeTest.cpp
    targetFileNodeTest.cpp
    tupleTypeTest.cpp
    typeRefTest.cpp
    typeSystemTest.cpp
    typeTest.cpp
    valueNamesTest.cpp
    valueHolderTest.cpp
    valueNodeTest.cpp
    valueTreeGenericTypeUtilsTest.cpp
   )


FILE(GLOB TestData "${CMAKE_CURRENT_SOURCE_DIR}/TestData/*")

MESSAGE("Copying BabelWiresLib TestData files to build folder")
FOREACH(file ${TestData})
    GET_FILENAME_COMPONENT(fileName ${file} NAME)
    CONFIGURE_FILE(${file} "${CMAKE_CURRENT_BINARY_DIR}/${fileName}" COPYONLY)
ENDFOREACH()

ADD_EXECUTABLE( babelWiresTests ${LIB_TESTS_SRCS} )
TARGET_INCLUDE_DIRECTORIES( babelWiresTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
TARGET_LINK_LIBRARIES(babelWiresTests Common testUtils BabelWiresLib libTestUtils gtest)
