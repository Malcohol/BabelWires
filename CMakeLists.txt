CMAKE_MINIMUM_REQUIRED( VERSION 3.25 )

###############################################################################
# Build the project
###############################################################################

PROJECT( BabelWires VERSION 0.0.1 )

SET(CMAKE_CXX_STANDARD 23)

include(CTest)

# MSVC's legacy preprocessor has bugs with variadic macros.
if(MSVC)
    add_compile_options(/Zc:preprocessor /utf-8)
endif()

option(BUILD_SHARED_LIBS "Build all BabelWires libraries as shared libraries" ON)
# Ensure that submodules (e.g. nodeeditor) respect the BUILD_SHARED_LIBS setting.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(BABELWIRES_PLUGIN_OUTPUT_DIR "${CMAKE_BINARY_DIR}/plugins" CACHE PATH
    "Directory where runtime-loadable BabelWires plugins are placed")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(AddPlugin)

if(BUILD_SHARED_LIBS)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
endif()

# Put all Windows shared-runtime artifacts in one place so executables and DLLs can find each other.
if(BUILD_SHARED_LIBS AND WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

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

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/tinyxml2/CMakeLists.txt"
   OR NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/yaml-cpp/CMakeLists.txt")
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
    set(YAML_CPP_BUILD_CONTRIB OFF)
    set(YAML_CPP_BUILD_TOOLS OFF)
    set(YAML_CPP_BUILD_TESTS OFF)
    set(YAML_CPP_INSTALL OFF)
    set(YAML_CPP_FORMAT_SOURCE OFF)
    set(YAML_BUILD_SHARED_LIBS OFF)
    add_subdirectory(extern/tinyxml2)
    add_subdirectory(extern/yaml-cpp)
    INCLUDE_DIRECTORIES(${tinyxml2_INCLUDE_DIRS})
    add_subdirectory(extern/libsndfile)
    add_subdirectory(extern/nodeeditor)
endblock()

# In shared builds, QtNodes is still linked as a static archive into BabelWiresQtUi.
# Hide QtNodes symbols at compile time (where supported) so they do not leak into
# libBabelWiresQtUi's exported symbol surface.
if(BUILD_SHARED_LIBS AND TARGET QtNodes)
    if(NOT MSVC)
        set_target_properties(QtNodes PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON)
    endif()
endif()

if(BUILD_TESTING)
    # Define a helper function to register test executables with CTest and set the appropriate environment variables for finding shared libraries.
    # Note: Registering individual tests with Ctest is far too slow, so we just use CTest as registry for the test executables, mainly for CI.
    function(register_test_executable targetName)
        add_test(NAME ${targetName} COMMAND $<TARGET_FILE:${targetName}>)

        if(NOT BUILD_SHARED_LIBS)
            return()
        endif()

        if(WIN32)
            set_tests_properties(${targetName} PROPERTIES
                ENVIRONMENT "PATH=$<TARGET_FILE_DIR:${targetName}>;$ENV{PATH}")
        elseif(APPLE)
            set_tests_properties(${targetName} PROPERTIES
                ENVIRONMENT "DYLD_LIBRARY_PATH=$<TARGET_FILE_DIR:${targetName}>:$ENV{DYLD_LIBRARY_PATH}")
        else()
            set_tests_properties(${targetName} PROPERTIES
                ENVIRONMENT "LD_LIBRARY_PATH=$<TARGET_FILE_DIR:${targetName}>:$ENV{LD_LIBRARY_PATH}")
        endif()
    endfunction()

    add_subdirectory(Tests/TestUtils)
    add_subdirectory(Tests/BaseLib)
    add_subdirectory(Tests/BabelWiresLib)
    add_subdirectory(Tests/BabelWiresLib/TestUtils)
endif()

# This is useful even if not building the tests.
add_subdirectory(Domains/TestDomain)
add_subdirectory(Domains/Music )

add_subdirectory(BabelWiresExe)
