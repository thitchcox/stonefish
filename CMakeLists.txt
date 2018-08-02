cmake_minimum_required(VERSION 2.6)
project(Stonefish)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DBT_EULER_DEFAULT_ZYX -DBT_USE_DOUBLE_PRECISION")

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Freetype REQUIRED)
find_package(Bullet REQUIRED)

include_directories(
    Library/include/Stonefish
    Library/3rdparty
    ${OPENGL_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${Eigen3_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
)

file(GLOB SOURCES "Library/src/*.cpp")
file(GLOB SOURCES_3RDPARTY_C "Library/src/*.c")
file(GLOB SOURCES_3RDPARTY_CXX "Library/src/*.cpp")

add_library(Stonefish SHARED ${SOURCES} ${SOURCES_3RDPARTY_C} ${SOURCES_3RDPARTY_CXX})

install(
    TARGETS Stonefish 
    LIBRARY DESTINATION /usr/local/lib
)

install(DIRECTORY Library/include/Stonefish DESTINATION /usr/local/include)
install(DIRECTORY Library/shaders DESTINATION /usr/local/share/Stonefish)
install(FILES Library/stonefish.pc DESTINATION /usr/local/lib/pkgconfig)