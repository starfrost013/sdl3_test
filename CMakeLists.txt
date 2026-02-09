# Capybara Wars Project
# A game for Windows & Linux - Copyright © 2025, 2026 starfrost

# CMakeLists.txt: Core CMake file

cmake_minimum_required (VERSION 3.10)

project("Capy")

add_subdirectory(external/sdl3 EXCLUDE_FROM_ALL)
add_subdirectory(external/sdl3_net EXCLUDE_FROM_ALL)

if (LINUX)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

# Add source to this project's executable.
add_executable (Capy 

"Capy.cpp"

# Tier 0
"core/tier0/memory/memory.cpp"

# Core
"core/core.cpp"
"core/core_input.cpp"

# Core - Convar
"core/cvar/cvar.cpp"

# Core - Network
"core/net/net_core.cpp"

# Core - Server
"core/net/server/server_core.cpp"

# Core - Client
"core/net/client/client_core.cpp"

"core/filesystem/filesystem.cpp"

# Entity
"data/entities/entity_player.cpp"
"data/entities/entity_world.cpp"

"data/levels/level.cpp"

"util/util.cpp"
"util/logging.cpp"
)


# Use C++23
set_property(TARGET Capy PROPERTY CXX_STANDARD 23)

# Static link
add_compile_options(Capy -static-libstdc++ -static-libgcc)

# Base include directories
include_directories(".")

message(${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}"/external/sdl3/include")
include_directories(${CMAKE_SOURCE_DIR}"/external/sdl3_net/include")

# Libraries and compiler specific stuff

#add_library(sdl3 STATIC IMPORTED)
#add_library(sdl3_net STATIC IMPORTED)

target_link_libraries(Capy PRIVATE SDL3::SDL3)
target_link_libraries(Capy PRIVATE SDL3_net::SDL3_net)

#set_target_properties(sdl3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/sdl3/x86_64-w64-mingw32/lib/libSDL3.dll.a)
#target_link_libraries(Capy sdl3)
#set_target_properties(sdl3_net PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/sdl3_net/libSDL3_net.dll.a)
#target_link_libraries(Capy sdl3_net)

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
  set(CAPY_INSTALL_DIR ${CMAKE_SOURCE_DIR}/build/output/win64)
  install(TARGETS Capy DESTINATION ${CAPY_INSTALL_DIR}) # probably easier to duplicate for now
elseif (LINUX)
  set(CAPY_INSTALL_DIR ${CMAKE_SOURCE_DIR}/build/output/linux64)
  install(TARGETS Capy DESTINATION ${CAPY_INSTALL_DIR}) # probably easier to duplicate for now

  # horrible hack for origin rpath (TEMP, we will static link everything)
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3/libSDL3.so DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3/libSDL3.so.0 DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3/libSDL3.so.0.4.1 DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3_net/libSDL3_net.so DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3_net/libSDL3_net.so.0 DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3_net/libSDL3_net.so.0.0.0 DESTINATION ${CAPY_INSTALL_DIR})
endif()

