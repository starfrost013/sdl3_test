# CMakeLists.txt: Core CMake file

cmake_minimum_required (VERSION 3.10)

project("Capy")

# Add source to this project's executable.
add_executable (Capy 

"Capy.cpp"

# Core
"core/core.cpp"
"core/core_input.cpp"

"core/filesystem/filesystem.cpp"

# Entity
"data/entities/entity_player.cpp"
"data/entities/entity_world.cpp"

"data/levels/level.cpp"

"util/util.cpp"
"util/logging.cpp"
)

# Base include directories
include_directories(".")
# Required for glad for now
include_directories(${CMAKE_SOURCE_DIR}"/external/sdl3/x86_64-w64-mingw32/include")
include_directories(${CMAKE_SOURCE_DIR}"/external/sdl3_net/include")

# Libraries and copiler specific stuff

add_library(sdl3 STATIC IMPORTED)

set_target_properties(sdl3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/sdl3/x86_64-w64-mingw32/lib/libSDL3.dll.a)
target_link_libraries(Capy sdl3)


if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  target_compile_definitions(Capy PRIVATE DEBUG)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  target_compile_definitions(Capy PRIVATE RELEASE)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
endif()


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Capy PROPERTY CXX_STANDARD 20)
endif()

#todo: linux
if (WIN32)
    file(COPY ${CMAKE_SOURCE_DIR}/external/sdl3/x86_64-w64-mingw32/bin/SDL3.dll DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY ${CMAKE_SOURCE_DIR}/external/sdl3_net/SDL3_net.dll DESTINATION ${CMAKE_BINARY_DIR})
endif()