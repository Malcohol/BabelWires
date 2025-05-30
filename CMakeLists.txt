CMAKE_MINIMUM_REQUIRED( VERSION 3.15 )

###############################################################################
# Build the project
###############################################################################

PROJECT( BabelWires )

#SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

SET( CMAKE_EDIT_COMMAND /usr/bin/cmake -i )
SET( CMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT TRUE )
SET(CMAKE_CXX_STANDARD 20)

# Lots of work will be required to support shared libs. Waiting to see whether C++ Modules will help.
set(BUILD_SHARED_LIBS OFF)
SET(gtest_force_shared_crt ON)

#SET(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebug)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

ADD_SUBDIRECTORY( Common )
ADD_SUBDIRECTORY( BabelWiresLib )

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/tinyxml2/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

FIND_PACKAGE( Qt5 REQUIRED COMPONENTS Core Widgets Gui OpenGL )
IF( Qt5_FOUND )
    ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    set(CMAKE_AUTOMOC ON)
    MESSAGE(${Qt5Widgets_INCLUDE_DIRS})
ENDIF( Qt5_FOUND )

ADD_SUBDIRECTORY( BabelWiresQtUi )

add_subdirectory(extern/tinyxml2)
INCLUDE_DIRECTORIES(${tinyxml2_INCLUDE_DIRS})

add_subdirectory(extern/googletest)
add_subdirectory(extern/libsndfile)

SET(USE_QT6 OFF CACHE INTERNAL "Tell nodeeditor to use Qt5")
add_subdirectory(extern/nodeeditor)

add_subdirectory(Tests/TestUtils)
add_subdirectory(Tests/Common)
add_subdirectory(Tests/BabelWiresLib)
add_subdirectory(Tests/BabelWiresLib/TestUtils)

add_subdirectory(Domains/TestDomain)
add_subdirectory(Domains/Music )

add_subdirectory(BabelWiresExe)
