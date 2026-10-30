#############################################
cmake_minimum_required(VERSION 3.16)
project(native_client LANGUAGES C CXX)

# compiler
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif()

add_compile_options(
    -O3
    -fomit-frame-pointer
    -Wall
    -fsigned-char
)

# libs
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(SDL REQUIRED)
#find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL_IMAGE REQUIRED SDL_image)
pkg_check_modules(VORBIS REQUIRED vorbisfile)

set(ENET_ROOT ${CMAKE_SOURCE_DIR}/src/engine/lib/enet)
add_library(enet STATIC IMPORTED)

set_target_properties(enet PROPERTIES
    IMPORTED_LOCATION "${ENET_ROOT}/libenet.a"
    INTERFACE_INCLUDE_DIRECTORIES "${ENET_ROOT}/include"
)

# src files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

list(REMOVE_ITEM SOURCES
    "${CMAKE_SOURCE_DIR}/src/engine/pch.cpp"
)

list(APPEND SOURCES
    src/engine/lib/glad/glad.c
)

add_executable(native_client ${SOURCES})

# precomp headers
target_precompile_headers(native_client PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/src/engine/pch.h>"
)

# include directories
target_include_directories(native_client PRIVATE
    src
    src/engine
    ${SDL_INCLUDE_DIR}
    ${OPENAL_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${SDL_IMAGE_INCLUDE_DIRS}
    ${VORBIS_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${ENET_INCLUDE_DIRS}
)

# set libs
target_link_libraries(native_client PRIVATE
    OpenGL::GL
    ${SDL_LIBRARY}
    OpenAL::OpenAL
    #CURL::libcurl
    ${SDL_IMAGE_LIBRARIES}
    ${VORBIS_LIBRARIES}
    enet
    X11
    z
)