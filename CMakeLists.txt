cmake_minimum_required(VERSION 3.9)

if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
	message(FATAL_ERROR
		"Building from within the source tree is not supported.\n")
endif()

###################################################################
# Core project definitions
###################################################################

project(toolbox VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # <-- This is used for YCM completer files

set(MY_DEBUG_FLAGS -ggdb3 -Og -D_GLIBCXX_DEBUG)
set(MY_RELEASE_FLAGS -O3 -mtune=native -march=native)
set(MY_WARNING_FLAGS_GNU
	-Wall -Wextra -Werror
	-fstrict-aliasing
	-Wstrict-aliasing=2
	-Wfloat-equal
	-Wno-missing-field-initializers
	-Wno-missing-braces
	-Wshadow
	-Wunreachable-code
	-Wredundant-decls
	-Wpointer-arith
	-Wundef
	-Wcast-align
	-Wswitch-enum)
set(MY_WARNING_FLAGS_Clang ${MY_DEFAULT_WARNING_FLAGS_GCC})

add_compile_options(
	"$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:DEBUG>>:${MY_DEBUG_FLAGS}>"
	"$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<CONFIG:RELEASE>>:${MY_RELEASE_FLAGS}>"
	)
add_compile_options("$<$<CXX_COMPILER_ID:GNU>:${MY_WARNING_FLAGS_GNU}>")
add_compile_options("$<$<CXX_COMPILER_ID:Clang>:${MY_WARNING_FLAGS_Clang}>")

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
		STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


###################################################################
# Find and use git to do the submodules business
###################################################################
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
	option(GIT_SUBMODULE "Check submodules during build" ON)
	if(GIT_SUBMODULE)
		message(STATUS "Submodule update")
		execute_process(
			COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif()
	endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/doctest/CMakeLists.txt")
	message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

set(DOCTEST_WITH_TESTS OFF CACHE BOOL "Build tests/examples" FORCE)
add_subdirectory(extern/doctest EXCLUDE_FROM_ALL)
add_subdirectory(extern/fmt EXCLUDE_FROM_ALL)

###################################################################
# CMake... we want is as Isystem...
###################################################################
function(target_link_libraries_system target)
	set(libs ${ARGN})
	foreach(lib ${libs})
		get_target_property(lib_include_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
		target_include_directories(${target} SYSTEM PRIVATE ${lib_include_dirs})
		target_link_libraries(${target} ${lib})
	endforeach(lib)
endfunction(target_link_libraries_system)

###################################################################
# Interesting things...
###################################################################

add_library(toolbox SHARED src/toolbox/FileMon.cpp)
target_include_directories(toolbox PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/extern/outcome/single-header>
	$<INSTALL_INTERFACE:include>
)
target_link_libraries(toolbox PRIVATE fmt::fmt-header-only)

include(CTest)
enable_testing()

#add_subdirectory(extern/phmap EXCLUDE_FROM_ALL)

add_library(test_main OBJECT test/main.cpp)
target_link_libraries(test_main doctest) # Why aren't you transitive?

add_executable(test_toolbox
	test/FileMon.cpp
	$<TARGET_OBJECTS:test_main>)
target_link_libraries(test_toolbox toolbox)
target_link_libraries_system(test_toolbox doctest)
target_link_libraries_system(test_toolbox fmt::fmt-header-only)
add_test(NAME test_toolbox COMMAND test_toolbox)

####################################################################
## Stuff needed to export as a CMake project
####################################################################

include(GNUInstallDirs)

target_include_directories(
	toolbox INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install(
	TARGETS toolbox
	EXPORT toolbox-config
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
	EXPORT toolbox-config
	NAMESPACE toolbox::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/toolbox)

install(
	DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/)

###################################################################
# This is needed to generate YCM completer files
###################################################################
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
		${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)
endif()

