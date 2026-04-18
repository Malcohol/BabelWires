SET( COMMON_SRCS
	Context/context.cpp
	DataContext/filePath.cpp
	Identifiers/identifierRegistry.cpp
	Identifiers/registeredIdentifier.cpp
	common.cpp
	libRegistration.cpp
	Version/version.cpp
	Cloning/cloneable.cpp
	Registry/registry.cpp
	Registry/fileTypeRegistry.cpp
	IO/dataSink.cpp
	IO/dataSource.cpp
	IO/fileDataSink.cpp
	IO/fileDataSource.cpp
	Math/rational.cpp
	Log/log.cpp
	Log/ostreamLogListener.cpp
	Log/unifiedLog.cpp
	Log/debugLogger.cpp
	Random/randomService.cpp
	uuid.cpp
	Serialization/deserializer.cpp
	Serialization/serializer.cpp
	Serialization/serializerDeserializerCommon.cpp
	Serialization/deserializationRegistry.cpp
	Serialization/XML/xmlSerializer.cpp
	Serialization/XML/xmlDeserializer.cpp
	Signal/signalSubscription.cpp
	BlockStream/blockStream.cpp
	BuildCompatibility/buildCompatibility.cpp
	PluginSupport/detail/pluginModuleOperations.cpp
	PluginSupport/pluginHandle.cpp
	PluginSupport/pluginOperations.cpp
	Utilities/unicodeUtils.cpp
   )

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated/BaseLib/Version)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Version/codebaseVersion.hpp.in
	${CMAKE_CURRENT_BINARY_DIR}/generated/BaseLib/Version/codebaseVersion_gen.hpp
	@ONLY
)

# TODO: Add install/export rules for generated BaseLib/Version/codebaseVersion_gen.hpp
# and associated CMake package metadata when external plugin support is introduced.

ADD_LIBRARY( BaseLib ${COMMON_SRCS} )
TARGET_INCLUDE_DIRECTORIES( BaseLib
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
		$<INSTALL_INTERFACE:include>
)
TARGET_LINK_LIBRARIES(BaseLib PUBLIC tinyxml2)

if(UNIX AND NOT APPLE)
	target_link_libraries(BaseLib PRIVATE dl)
endif()

if(BUILD_SHARED_LIBS)
	set_target_properties(BaseLib PROPERTIES
		CXX_VISIBILITY_PRESET hidden
		VISIBILITY_INLINES_HIDDEN ON)
	target_compile_definitions(BaseLib PRIVATE BASELIB_EXPORTS)
else()
	target_compile_definitions(BaseLib PUBLIC BASELIB_STATIC)
endif()
