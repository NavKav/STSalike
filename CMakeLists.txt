cmake_minimum_required(VERSION 3.9)
project(STSalike CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(_USE_MATH_DEFINES)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Ajouter l'option -mwindows si l'on veut supprimer la console pour le release
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -mwindows -municode")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

find_package(SDL2 REQUIRED)
find_package(SDL2_Image REQUIRED)
find_package(SDL2_ttf REQUIRED)

if(WIN32)
        find_library(SDL2_MAIN_LIBRARY_RELEASE
                NAMES SDL2main
                PATH_SUFFIXES lib lib/x64 lib/x86
                PATHS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib"
                "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib"
        )
        find_library(SDL2_MAIN_LIBRARY_DEBUG
                NAMES SDL2maind
                PATH_SUFFIXES lib lib/x64 lib/x86
                PATHS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/lib"
                "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/debug/lib"
        )

        include(SelectLibraryConfigurations)
        select_library_configurations(SDL2_MAIN)

        message(STATUS "SDL2_MAIN_LIBRARY: ${SDL2_MAIN_LIBRARY}")
else()
        set(SDL2_MAIN_LIBRARY "")
endif()

file(GLOB_RECURSE SOURCES
        src/*.cpp
        src/*/*.cpp
        src/*/*/*.cpp
        src/*/*/*/*.cpp
        src/*/*/*/*/*.cpp
)

add_executable(STSalike ${SOURCES}
        src/core/InputDefinition.h)

target_include_directories(STSalike PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${SDL2_INCLUDE_DIR}
        ${SDL2_IMAGE_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR}
)

target_link_libraries(STSalike PRIVATE
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_LIBRARIES}
)

if(WIN32)
        target_link_libraries(STSalike PRIVATE
                ${SDL2_MAIN_LIBRARY}
                wsock32 ws2_32
        )
elseif(UNIX)
        target_link_libraries(STSalike PRIVATE pthread)
endif()