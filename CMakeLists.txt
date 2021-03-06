cmake_minimum_required(VERSION 3.10)
project(Stonefish VERSION 1.1.0)

configure_file(version.h.in version.h)
configure_file(stonefish.pc.in stonefish.pc)

# Options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_TESTS "Build applications testing different features of the Stonefish library" OFF)

# Set up CMAKE flags
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-DBT_EULER_DEFAULT_ZYX -DBT_USE_DOUBLE_PRECISION")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(OpenGL_GL_PREFERENCE "GLVND")

# Find required libraries
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)

# Add include directories
include_directories(
    ${PROJECT_BINARY_DIR}
    Library/include
    3rdparty
    ${OPENGL_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

# Build list of library source files
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Library/src/*.cpp")
file(GLOB_RECURSE SOURCES_3RD "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/*.cpp")

# Define targets
if(BUILD_TESTS)
    # Create tests and use library locally (has to be disabled when installing system-wide!)
    add_library(Stonefish_test SHARED ${SOURCES} ${SOURCES_3RD})
    target_link_libraries(Stonefish_test ${FREETYPE_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})
    add_definitions(-DSHADER_DIR_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/Library/shaders/\") #Modifies shader path of the library
    add_subdirectory(Tests)
else()
    # Create shared library to be installed system-wide
    add_library(Stonefish SHARED ${SOURCES} ${SOURCES_3RD})
    target_link_libraries(Stonefish ${FREETYPE_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

    # Install library in the system
    install(
        TARGETS Stonefish 
        LIBRARY DESTINATION /usr/local/lib
    )
    
    # Install Stonefish headers
    install(DIRECTORY Library/include/ DESTINATION /usr/local/include/Stonefish)
    
    # Install 3rdparty headers
    file(GLOB_RECURSE INCLUDES_3RD "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/*.h")
    foreach(f ${INCLUDES_3RD})
        file(RELATIVE_PATH frel "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty" ${f})
        get_filename_component(dir ${frel} DIRECTORY)
        install(FILES ${f} DESTINATION /usr/local/include/Stonefish/${dir})
    endforeach()
    
    #Install other files
    install(DIRECTORY Library/shaders/ DESTINATION /usr/local/share/Stonefish/shaders)
    install(FILES ${PROJECT_BINARY_DIR}/version.h DESTINATION /usr/local/include/Stonefish)
    install(FILES ${PROJECT_BINARY_DIR}/stonefish.pc DESTINATION /usr/local/lib/pkgconfig)
endif()