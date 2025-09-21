SET( COMMON_SRCS
	DataContext/filePath.cpp
	exceptions.cpp
	Identifiers/identifierRegistry.cpp
	Identifiers/registeredIdentifier.cpp
	types.cpp
	Cloning/cloneable.cpp
	Registry/registry.cpp
	Registry/fileTypeRegistry.cpp
	IO/dataSource.cpp
	IO/fileDataSource.cpp
	IO/outFileStream.cpp
	Math/rational.cpp
	Log/log.cpp
	Log/ostreamLogListener.cpp
	Log/unifiedLog.cpp
	Log/debugLogger.cpp
	uuid.cpp
	Serialization/deserializer.cpp
	Serialization/serializer.cpp
	Serialization/serializerDeserializerCommon.cpp
	Serialization/automaticDeserializationRegistry.cpp
	Serialization/XML/xmlSerializer.cpp
	Serialization/XML/xmlDeserializer.cpp
	Signal/signalSubscription.cpp
	BlockStream/blockStream.cpp
	Utilities/result.cpp
	Utilities/unicodeUtils.cpp
   )

ADD_LIBRARY( Common ${COMMON_SRCS} )
TARGET_INCLUDE_DIRECTORIES( Common PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. )
TARGET_LINK_LIBRARIES(Common PUBLIC tinyxml2)
