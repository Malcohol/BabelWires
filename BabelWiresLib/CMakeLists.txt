SET( BABELWIRESLIB_SRCS
	Project/project.cpp
	Project/FeatureElements/featureElement.cpp
	Project/FeatureElements/featureElementData.cpp
	Project/FeatureElements/processorElement.cpp
	Project/FeatureElements/sourceFileElement.cpp
	Project/FeatureElements/targetFileElement.cpp
	Project/FeatureElements/fileElement.cpp
	Project/FeatureElements/failedFeature.cpp
	Project/FeatureElements/editTree.cpp
	Project/FeatureElements/contentsCache.cpp
	Project/FeatureElements/sourceFileElementData.cpp
	Project/FeatureElements/targetFileElementData.cpp
	Project/FeatureElements/processorElementData.cpp
	Project/Modifiers/modifier.cpp
	Project/Modifiers/modifierData.cpp
	Project/Modifiers/localModifier.cpp
	Project/Modifiers/arraySizeModifier.cpp
	Project/Modifiers/arraySizeModifierData.cpp
	Project/Modifiers/connectionModifier.cpp
	Project/Modifiers/connectionModifierData.cpp
	Project/Modifiers/activateOptionalsModifierData.cpp
	Project/Modifiers/mapValueAssignmentData.cpp
	Project/uiPosition.cpp
	Project/projectData.cpp
	ProjectExtra/projectObserver.cpp
	ProjectExtra/connectionDescription.cpp
	ProjectExtra/projectUtilities.cpp
	Enums/enum.cpp
	FileFormat/sourceFileFormat.cpp
	FileFormat/targetFileFormat.cpp
	FileFormat/fileFeature.cpp
	FileFormat/filePath.cpp
	Maps/mapProject.cpp
	Maps/typeSystem.cpp
	Maps/Commands/addEntryToMapCommand.cpp
	Maps/Commands/removeEntryFromMapCommand.cpp
	Maps/mapSerialization.cpp
	Maps/mapData.cpp
	Maps/MapEntries/mapEntryData.cpp
	Maps/mapProjectEntry.cpp
	Processors/parallelProcessor.cpp
	Processors/processorFactoryRegistry.cpp
	Processors/processor.cpp
	Processors/processorFactory.cpp
	Processors/commonProcessor.cpp
	Features/features.cpp
	Features/recordFeature.cpp
	Features/recordWithOptionalsFeature.cpp
	Features/arrayFeature.cpp
	Features/enumFeature.cpp
	Features/numericFeature.cpp
	Features/stringFeature.cpp
	Features/mapFeature.cpp
	Features/Path/featurePath.cpp
	Features/Path/pathStep.cpp
	Features/Utilities/featureXml.cpp
	Features/rootFeature.cpp
	Serialization/projectBundle.cpp
	Project/Commands/activateOptionalCommand.cpp
	Project/Commands/addElementCommand.cpp
	Project/Commands/deactivateOptionalCommand.cpp
	Project/Commands/removeElementCommand.cpp
	Project/Commands/moveElementCommand.cpp
	Project/Commands/addModifierCommand.cpp
	Project/Commands/removeModifierCommand.cpp
	Project/Commands/addEntryToArrayCommand.cpp
	Project/Commands/removeEntryFromArrayCommand.cpp
	Project/Commands/changeFileCommand.cpp
	Project/Commands/pasteElementsCommand.cpp
	Project/Commands/setExpandedCommand.cpp
	Project/Commands/removeAllEditsCommand.cpp
	Project/Commands/removeFailedModifiersCommand.cpp
	Project/Commands/resizeElementCommand.cpp
	Project/Commands/setArraySizeCommand.cpp
	ValueNames/valueNames.cpp
	ValueNames/sparseValueNamesImpl.cpp
	ValueNames/contiguousValueNamesImpl.cpp
   )

ADD_LIBRARY( BabelWiresLib ${BABELWIRESLIB_SRCS} )
TARGET_INCLUDE_DIRECTORIES( BabelWiresLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. )
TARGET_LINK_LIBRARIES(BabelWiresLib PUBLIC tinyxml2 Common)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# Required when using <execution> with gcc.
	TARGET_LINK_LIBRARIES(BabelWiresLib PUBLIC tbb)
endif()