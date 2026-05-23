SET( BABELWIRES_EXE_SCRS
	main.cpp
	babelWiresOptions.cpp
   )

ADD_EXECUTABLE( babelWires ${BABELWIRES_EXE_SCRS} )
TARGET_INCLUDE_DIRECTORIES( babelWires PRIVATE ${PROJECT_SOURCE_DIR} )

if(BUILD_SHARED_LIBS)
	TARGET_LINK_LIBRARIES( babelWires musicLibUi BabelWiresQtUi BabelWiresLib BaseLib libTestDomain)
	target_compile_definitions(babelWires PRIVATE BABELWIRES_DEFAULT_PLUGIN_DIR=\"${BABELWIRES_PLUGIN_OUTPUT_DIR}\"
		BABELWIRES_SHARED_BUILD=1)
else()
	TARGET_LINK_LIBRARIES( babelWires SmfLib musicLibUi BabelWiresQtUi BabelWiresLib BaseLib libTestDomain TestPluginLib)
	target_compile_definitions(babelWires PRIVATE BABELWIRES_SHARED_BUILD=0)
endif()
