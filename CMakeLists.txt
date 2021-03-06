cmake_minimum_required(VERSION 2.6)
project(yage CXX)


# Setting the project's Cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

cmake_policy(PUSH)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


function(add_unit_test target target_test)
  get_property(_target_output_directory TARGET ${target} PROPERTY RUNTIME_OUTPUT_DIRECTORY)
  #  message(STATUS "add_unit_test: ${_target_output_directory}/${target_test}")
  set(UNIT_TEST_TARGETS ${UNIT_TEST_TARGETS} ${target_test} PARENT_SCOPE)
  add_test(target ${_target_output_directory}/${target_test})
endfunction(add_unit_test)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -g")

# Define sources
set(YAGE_SRC
  src/engine/game_engine.cpp
  src/engine/game_state.cpp
  src/engine/state_machine.cpp
  src/engine/states/splash_screen.cpp
	
  src/graphics/drawable.abstract.cpp
     	
  src/events/event_manager.cpp
	
  src/graphics/graphics_manager.cpp
  src/graphics/texture_manager.cpp
  src/graphics/texture.cpp
  src/graphics/spritesheet.cpp
  src/graphics/font_manager.cpp
  src/graphics/font.cpp


  src/input/input_manager.cpp
  src/input/mouse.cpp
  
  src/widgets/label.cpp
  src/widgets/button.cpp

  src/utility.cpp
)



# Targets
add_library(yage SHARED ${YAGE_SRC})
set_property(TARGET yage PROPERTY CXX_STANDARD 17)
set_property(TARGET yage PROPERTY POSITION_INDEPENDENT_CODE ON)
set_target_properties(yage
	PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/lib"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/lib"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/bin")
	
cmake_policy(SET CMP0076 NEW)
target_sources(yage
  PUBLIC
    include/yage/events/event_listener.hpp
    include/yage/graphics/bounding_box.interface.hpp
    
    include/yage.hpp

    include/yage/utility.hpp

    include/yage/engine/game_engine.hpp
    include/yage/engine/game_state.hpp
    include/yage/engine/runnable.hpp
    include/yage/engine/state_machine.hpp

    include/yage/events/event_manager.hpp

    include/yage/graphics/font_manager.hpp
    include/yage/graphics/graphics_manager.hpp
    include/yage/graphics/texture_manager.hpp

    include/yage/graphics/font.hpp
    include/yage/graphics/spritesheet.hpp
    include/yage/graphics/texture.hpp

  PRIVATE
    ${YAGE_SRC}  
)


# Dependencies
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
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

#  SDL dependencies.
set(BUILD_SHARED_LIBRARIES ON)
find_package(SDL2)
if (NOT SDL2_FOUND)
  message(FATAL_ERROR "SDL2 was not fond on the system")
endif()
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(yage ${SDL2_LIBRARY})

find_package(SDL2_image)
if (NOT SDL2_IMAGE_FOUND)
  message(FATAL_ERROR "SDL2_image was not found on the system")
endif()
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
target_link_libraries(yage ${SDL2_IMAGE_LIBRARIES})

find_package(SDL2_ttf REQUIRED)
if (NOT SDL2_TTF_FOUND)
  message(FATAL_ERROR "SDL2_TTF was not found on the system")
endif()
include_directories(${SDL2_TTF_INCLUDE_DIRS})
target_link_libraries(yage ${SDL2_TTF_LIBRARIES})

#  GTest for unit testing
set(GTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/googletest/googletest")
add_subdirectory(${GTEST_ROOT} EXCLUDE_FROM_ALL)
include_directories("${gtest_SOURCE_DIR}/include")

enable_testing()
# Tests
add_executable(graphics_test 
	tests/graphics/graphics_manager.unit.cpp
	tests/graphics/spritesheet.cpp)
add_executable(state_machine_test tests/engine/state_machine.unit.cpp)

target_link_libraries(graphics_test gtest_main yage ${SDL2_IMAGE_LIBRARIES} ${SDL2_LIBRARY})
target_link_libraries(state_machine_test gtest gtest_main yage)

add_dependencies(graphics_test gtest gtest_main yage)
add_dependencies(state_machine_test gtest gtest_main yage)

set_property(TARGET graphics_test PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/tests")
set_property(TARGET state_machine_test PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output/tests")

add_unit_test(graphics_test graphics_test)
add_unit_test(state_machine_test state_machine_test)


# Sources
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# Helper variables for client projects
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  set(SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIR} PARENT_SCOPE)
  
  set(SDL2_LIBRARY ${SDL2_LIBRARY} PARENT_SCOPE)

  set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIRS} PARENT_SCOPE)
  set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARIES} PARENT_SCOPE)

  set(SDL2_TTF_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIRS} PARENT_SCOPE)
  set(SDL2_TTF_LIBRARIES ${SDL2_TTF_LIBRARIES} PARENT_SCOPE)

  set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} PARENT_SCOPE)
  set(GTEST_BOTH_LIBRARIES ${GTEST_BOTH_LIBRARIES} PARENT_SCOPE)

  set(YAGE_DEPENDENCIES_INCLUDE_DIRS ${SDL2_INCLUDE_DIR}:${SDL2_IMAGE_INCLUDE_DIRS}:${SDL2_TTF_INCLUDE_DIRS}:${GTEST_INCLUDE_DIRS} PARENT_SCOPE)
endif()


add_custom_target( all_tests ALL
                   DEPENDS ${UNIT_TEST_TARGETS}
)
add_custom_command(TARGET all_tests
                   COMMENT "Run tests"
                   POST_BUILD COMMAND ctest ARGS --output-on-failure
                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

cmake_policy(POP)
