SET( ALSA_SRCS
	alsaAudioDest.cpp
	alsaAudioSource.cpp
	alsaCommon.cpp
	alsaAudioInterface.cpp
	alsaInit.cpp
	)
	
ADD_LIBRARY( BabelWires_Alsa ${ALSA_SRCS} )
TARGET_INCLUDE_DIRECTORIES( BabelWires_Alsa PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../.. )
TARGET_LINK_LIBRARIES(BabelWires_Alsa asound)
