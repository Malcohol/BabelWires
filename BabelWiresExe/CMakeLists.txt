SET( SEQWIRES_SRCS
	main.cpp
	babelWiresOptions.cpp
   )

ADD_EXECUTABLE( babelWires ${SEQWIRES_SRCS} )
TARGET_INCLUDE_DIRECTORIES( babelWires PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. )
TARGET_LINK_LIBRARIES( babelWires SmfLib SeqWiresLibUi BabelWiresQtUi BabelWiresLib Common tinyxml2 libTestDomain)
