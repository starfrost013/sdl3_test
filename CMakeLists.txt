# Capybara Wars Project
# A game for Windows & Linux - Copyright © 2025, 2026 starfrost

# CMakeLists.txt: Core CMake file

cmake_minimum_required (VERSION 3.10)

option(DEDICATED_SERVER "Dedicated server only" OFF)

project("Capy")

add_subdirectory(external/sdl3 EXCLUDE_FROM_ALL)
add_subdirectory(external/sdl3_net EXCLUDE_FROM_ALL)
add_subdirectory(external/squirrel EXCLUDE_FROM_ALL)

if (LINUX)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

# Add source to this project's executable.
add_executable (Capy 

"Capy.cpp"

# Tier 0
"core/tier0/memory/memory.cpp"

# Core
"core/game.cpp"
"core/game_input.cpp"

# Core - command line
"core/cmdline/cmdline.cpp"

# Core - command system
"core/command/command.cpp"
"core/command/commands_basic.cpp"
"core/command/commands_exec.cpp"
"core/command/commands_memory.cpp"

# Core - Convar
"core/cvar/cvar.cpp"

# Core - FS
"core/filesystem/filesystem.cpp"

# Core - Render
"core/render/render.cpp"

# Core - Asset System
"core/render/assets/asset_texture.cpp"

# Core - script
"core/script/script_core.cpp"

# Net - Core
"net/net_core.cpp"
"net/net_message.cpp"
"net/net_packetfactory.cpp"

# Net - Server
"net/server/server_auth.cpp"
"net/server/server_console.cpp"
"net/server/server_core.cpp"

# Net - Client
"net/client/client_auth.cpp"
"net/client/client_core.cpp"
"net/client/client_rpc.cpp"

# Entity
"entity/entity_netidentity.cpp"
"entity/entity_physical.cpp"
"entity/entity_player.cpp"

# World
"world/world.cpp"
"world/world_save.cpp"

"util/util.cpp"
"util/logging.cpp"
)

# Use C++23
set_property(TARGET Capy PROPERTY CXX_STANDARD 23)

# Base include directories
include_directories(".")

message(${CMAKE_SOURCE_DIR})

# Libraries and compiler specific stuff (prefer submodules)

include_directories(${CMAKE_SOURCE_DIR}"/external/sdl3/include")
include_directories(${CMAKE_SOURCE_DIR}"/external/sdl3_net/include")
include_directories(${CMAKE_SOURCE_DIR}"/external/stbi")
include_directories(${CMAKE_SOURCE_DIR}"/external/squirrel/include")

target_link_libraries(Capy PRIVATE SDL3::SDL3)
target_link_libraries(Capy PRIVATE SDL3_net::SDL3_net)
target_link_libraries(Capy PRIVATE squirrel_static)
target_link_libraries(Capy PRIVATE sqstdlib_static)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  target_compile_definitions(Capy PRIVATE DEBUG)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  target_compile_definitions(Capy PRIVATE RELEASE)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")

endif()

# Enable dedicated server if required
if(DEDICATED_SERVER)
  target_compile_definitions(Capy PRIVATE DEDICATED)
endif()

# Static link
add_compile_options(Capy -static-libstdc++ -static-libgcc)
add_compile_options(SDL3::SDL3 -static-libstdc++ -static-libgcc)
add_compile_options(SDL3::SDL3_net -static-libstdc++ -static-libgcc)

#todo: linux
if (WIN32)  
  set(CAPY_INSTALL_DIR ${CMAKE_SOURCE_DIR}/build/output/win64)
  install(TARGETS Capy DESTINATION ${CAPY_INSTALL_DIR}) # probably easier to duplicate for now

  # horrible until we figure out how to statically link sdl3
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3/SDL3.dll DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3_net/SDL3_net.dll DESTINATION ${CAPY_INSTALL_DIR})

elseif (LINUX)
  set(CAPY_INSTALL_DIR ${CMAKE_SOURCE_DIR}/build/output/linux64)
  install(TARGETS Capy DESTINATION ${CAPY_INSTALL_DIR}) # probably easier to duplicate for now

  # horrible hack for origin rpath (TEMP, we will static link everything)
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3/libSDL3.so DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3/libSDL3.so.0 DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3/libSDL3.so.0.4.3 DESTINATION ${CAPY_INSTALL_DIR}) #ireally need to fix this (SDL 3.4.3)
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3_net/libSDL3_net.so DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3_net/libSDL3_net.so.0 DESTINATION ${CAPY_INSTALL_DIR})
  install(FILES ${CMAKE_SOURCE_DIR}/build/external/sdl3_net/libSDL3_net.so.0.0.0 DESTINATION ${CAPY_INSTALL_DIR})
endif()

