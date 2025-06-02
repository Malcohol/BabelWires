SET( BABELWIRES_EXE_SCRS
	main.cpp
	babelWiresOptions.cpp
   )

ADD_EXECUTABLE( babelWires ${BABELWIRES_EXE_SCRS} )
TARGET_INCLUDE_DIRECTORIES( babelWires PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. )
TARGET_LINK_LIBRARIES( babelWires SmfLib musicLibUi BabelWiresQtUi BabelWiresLib Common tinyxml2 libTestDomain)
