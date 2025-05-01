# Step7 Engine
# Copyright © 2024, 2025 starfrost
#
# CMakeLists.txt: Core CMake file

cmake_minimum_required (VERSION 3.8)

project("DDATest")

# Add source to this project's executable.
add_executable (DDATest 

"ddatest.cpp"

# Core
"core/core.cpp"
"core/core_input.cpp"
"core/core_renderer.cpp"

# Entity
"entities/entity_player.cpp"

# Level
"data/levels/levels_temp.cpp"

)

# Base include directories
include_directories(".")
# Required for glad for now
include_directories(${CMAKE_SOURCE_DIR}"/external/sdl3/x86_64-w64-mingw32/include")

# Libraries and copiler specific stuff

add_library(sdl3 STATIC IMPORTED)

set_target_properties(sdl3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/sdl3/x86_64-w64-mingw32/lib/libSDL3.dll.a)
target_link_libraries(DDATest sdl3)


# Build system-specific files
#if (WIN32)
#	target_sources(DDATest PUBLIC "sys/windows/sys_win32.cpp")
#else()
#	target_sources(DDATest PUBLIC "sys/linux/sys_linux.cpp")
#endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DDATest PROPERTY CXX_STANDARD 20)
endif()

#todo: linux
if (WIN32)
    file(COPY ${CMAKE_SOURCE_DIR}/external/sdl3/x86_64-w64-mingw32/bin/SDL3.dll DESTINATION ${CMAKE_BINARY_DIR})
endif()