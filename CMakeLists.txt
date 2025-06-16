cmake_minimum_required(VERSION 3.9)
project(STSalike CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(_USE_MATH_DEFINES)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

if(WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/SDL2/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_Image REQUIRED)
find_package(SDL2_ttf REQUIRED)

if(NOT SDL2_LIBRARIES)
        set(SDL2_LIBRARIES "C:/Users/navid/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/debug/lib/libSDL2d.dll.a")
endif()
if(NOT SDL2_IMAGE_LIBRARIES)
        set(SDL2_IMAGE_LIBRARIES "C:/Users/navid/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/debug/lib/libSDL2_imaged.dll.a")
endif()
if(NOT SDL2_TTF_LIBRARIES)
        set(SDL2_TTF_LIBRARIES "C:/Users/navid/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/debug/lib/libSDL2_ttfd.dll.a")
endif()

set(SDL2_MAIN_LIBRARY "C:/Users/navid/.vcpkg-clion/vcpkg/installed/x64-mingw-dynamic/debug/lib/libSDL2maind.a")

file(GLOB_RECURSE SOURCES
        src/*.cpp
        src/*/*.cpp
        src/*/*/*.cpp
        src/*/*/*/*.cpp
        src/*/*/*/*/*.cpp
)

add_executable(STSalike ${SOURCES})

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
        ${SDL2_MAIN_LIBRARY}
)

if(WIN32)
        target_link_libraries(STSalike PRIVATE wsock32 ws2_32)
elseif(UNIX)
        target_link_libraries(STSalike PRIVATE pthread)
endif()