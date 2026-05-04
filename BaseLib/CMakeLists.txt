SET( COMMON_SRCS
	Context/context.cpp
	DataContext/filePath.cpp
	DataContext/dataVisitable.cpp
	Identifiers/identifierRegistry.cpp
	Identifiers/registeredIdentifier.cpp
	common.cpp
	libRegistration.cpp
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
	productInfo.cpp
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
	Utilities/unicodeUtils.cpp
   )

ADD_LIBRARY( BaseLib ${COMMON_SRCS} )
TARGET_INCLUDE_DIRECTORIES( BaseLib
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
)
TARGET_LINK_LIBRARIES(BaseLib PUBLIC tinyxml2)

if(BUILD_SHARED_LIBS)
	set_target_properties(BaseLib PROPERTIES
		CXX_VISIBILITY_PRESET hidden
		VISIBILITY_INLINES_HIDDEN ON)
	target_compile_definitions(BaseLib PRIVATE BASELIB_EXPORTS)
else()
	target_compile_definitions(BaseLib PUBLIC BASELIB_STATIC)
endif()
