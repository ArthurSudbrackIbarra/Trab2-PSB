cmake_minimum_required (VERSION 3.1.0)

project(seamcarving)

SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

# Suppress warnings of the deprecation of glut functions on macOS.
if(APPLE)
    add_definitions(-Wno-deprecated-declarations -Os)
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(INCLUDE_DIRS ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR} lib/SOIL)
set(LIBRARIES ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

include_directories(${INCLUDE_DIRS})

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/lib/SOIL/*.c)
file(GLOB INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/lib/SOIL*.h)

add_executable(seamcarving ${SOURCE_FILES} ${INCLUDE_FILES})

target_link_libraries(seamcarving ${LIBRARIES})

