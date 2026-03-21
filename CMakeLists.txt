CMAKE_MINIMUM_REQUIRED( VERSION 3.25 )

###############################################################################
# Build the project
###############################################################################

PROJECT( BabelWires )

SET(CMAKE_CXX_STANDARD 23)

include(CTest)

# MSVC's legacy preprocessor has bugs with variadic macros.
if(MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

option(BUILD_SHARED_LIBS "Build all BabelWires libraries as shared libraries" OFF)
# Ensure that submodules (e.g. nodeeditor) respect the BUILD_SHARED_LIBS setting.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

SET(gtest_force_shared_crt ON)

#SET(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebug)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

ADD_SUBDIRECTORY( BaseLib )
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

FIND_PACKAGE( Qt6 REQUIRED COMPONENTS Core Widgets Gui OpenGL )
IF( Qt6_FOUND )
    ADD_DEFINITIONS(${Qt6Widgets_DEFINITIONS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")
    include_directories(${Qt6Widgets_INCLUDE_DIRS})
    set(CMAKE_AUTOMOC ON)
    MESSAGE(${Qt6Widgets_INCLUDE_DIRS})
ENDIF( Qt6_FOUND )

ADD_SUBDIRECTORY( BabelWiresQtUi )

if(BUILD_TESTING)
    block(SCOPE_FOR VARIABLES)
        set(BUILD_SHARED_LIBS OFF)
        add_subdirectory(extern/googletest)
    endblock()
endif()

# Force external libraries to static and disable their tests.
block(SCOPE_FOR VARIABLES)
    set(BUILD_SHARED_LIBS OFF)
    set(BUILD_TESTING OFF)
    set(tinyxml2_BUILD_TESTING OFF)
    add_subdirectory(extern/tinyxml2)
    INCLUDE_DIRECTORIES(${tinyxml2_INCLUDE_DIRS})
    add_subdirectory(extern/libsndfile)
    add_subdirectory(extern/nodeeditor)
endblock()

if(BUILD_TESTING)
    include(GoogleTest)
    add_subdirectory(Tests/TestUtils)
    add_subdirectory(Tests/BaseLib)
    add_subdirectory(Tests/BabelWiresLib)
    add_subdirectory(Tests/BabelWiresLib/TestUtils)
endif()

# This is useful even if not building the tests.
add_subdirectory(Domains/TestDomain)
add_subdirectory(Domains/Music )
add_subdirectory(BabelWiresExe)
