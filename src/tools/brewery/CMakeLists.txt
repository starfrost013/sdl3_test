# Brewery 
# Generates beerfiles for Capybara Wars
# Copyright © 2025, 2026 starfrost

# CMakeLists.txt: Core CMake file

cmake_minimum_required (VERSION 3.10)

project("Brewery")

if (LINUX)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

# Add source to this project's executable.
add_executable (Brewery

"Brewery.cpp"

# consider moving these into their own library which could be linked by the project.
# maybe later

# Tier 0
"memory/memory.cpp"

# Core - command line
"cmdline/cmdline.cpp"

"util/util.cpp"
"util/logging.cpp"
)

# Use C++23
set_property(TARGET Brewery PROPERTY CXX_STANDARD 23)

# Base include directories
include_directories(".")

# Libraries and compiler specific stuff (prefer submodules)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  target_compile_definitions(Brewery PRIVATE DEBUG)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  target_compile_definitions(Brewery PRIVATE RELEASE)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
endif()

if (WIN32)  
  set(BREWERY_INSTALL_DIR ${CMAKE_SOURCE_DIR}/../../../build/output/win64/tools)
  install(TARGETS Brewery DESTINATION ${BREWERY_INSTALL_DIR}) # probably easier to duplicate for now
elseif (LINUX)
  set(BREWERY_INSTALL_DIR ${CMAKE_SOURCE_DIR}/../../../build/output/linux64/tools)
  install(TARGETS Brewery DESTINATION ${BREWERY_INSTALL_DIR}) # probably easier to duplicate for now
endif()

