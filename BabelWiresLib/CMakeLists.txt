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
	Project/uiPosition.cpp
	Project/projectData.cpp
	ProjectExtra/projectObserver.cpp
	ProjectExtra/connectionDescription.cpp
	ProjectExtra/projectUtilities.cpp
	FileFormat/sourceFileFormat.cpp
	FileFormat/targetFileFormat.cpp
	FileFormat/fileFeature.cpp
	FileFormat/filePath.cpp
	Processors/parallelProcessor.cpp
	Processors/processorFactoryRegistry.cpp
	Processors/processor.cpp
	Processors/processorFactory.cpp
	Processors/commonProcessor.cpp
	Features/features.cpp
	Features/recordFeature.cpp
	Features/recordWithOptionalsFeature.cpp
	Features/arrayFeature.cpp
	Features/numericFeature.cpp
	Features/stringFeature.cpp
	Features/Path/fieldIdentifier.cpp
	Features/Path/featurePath.cpp
	Features/Path/fieldNameRegistry.cpp
	Features/Path/fieldName.cpp
	Features/Path/pathStep.cpp
	Features/Utilities/featureXml.cpp
	Serialization/projectSerialization.cpp
	Serialization/projectBundle.cpp
	Commands/activateOptionalCommand.cpp
	Commands/commands.cpp
	Commands/addElementCommand.cpp
	Commands/deactivateOptionalCommand.cpp
	Commands/removeElementCommand.cpp
	Commands/moveElementCommand.cpp
	Commands/addModifierCommand.cpp
	Commands/removeModifierCommand.cpp
	Commands/addEntryToArrayCommand.cpp
	Commands/removeEntryFromArrayCommand.cpp
	Commands/changeFileCommand.cpp
	Commands/pasteElementsCommand.cpp
	Commands/commandManager.cpp
	Commands/setExpandedCommand.cpp
	Commands/removeAllEditsCommand.cpp
	Commands/removeFailedModifiersCommand.cpp
	Commands/resizeElementCommand.cpp
	ValueNames/valueNames.cpp
	ValueNames/valueNamesImpl.cpp
   )

ADD_LIBRARY( BabelWiresLib ${BABELWIRESLIB_SRCS} )
TARGET_INCLUDE_DIRECTORIES( BabelWiresLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. )
TARGET_LINK_LIBRARIES(BabelWiresLib PUBLIC tinyxml2 Common tbb)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# Required when using <execution> with gcc.
	TARGET_LINK_LIBRARIES(BabelWiresLib PUBLIC tbb)
endif()