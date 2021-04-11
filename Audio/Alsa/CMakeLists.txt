SET( ALSA_SRCS
	alsaAudioDest.cpp
	alsaAudioSource.cpp
	alsaCommon.cpp
	alsaAudioInterface.cpp
	alsaInit.cpp
	)
	
ADD_LIBRARY( BabelWires_Alsa ${ALSA_SRCS} )
TARGET_LINK_LIBRARIES(BabelWires_Alsa asound)
