cmake_minimum_required(VERSION 2.6)

PROJECT(AIVI-Lab1)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "CMAKE_BUILD_TYPE: Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(TARGET "lab2")

SET(SRC lab2.cpp utils.hpp utils.cpp blockmatching.cpp blockmatching.hpp)

ADD_DEFINITIONS(-Wall -std=c++11)

#SET("OpenCV_DIR" "~/opencv-2.4.9/build/")
#find_package(OpenCV REQUIRED)
#set(OpenCV_FOUND 1)
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

ADD_EXECUTABLE(${TARGET} ${SRC})
TARGET_LINK_LIBRARIES(${TARGET} ${FFMPEG_LIBRARIES} ${OpenCV_LIBS})
