cmake_minimum_required(VERSION 2.6)

PROJECT(AIVI-Lab3)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING "CMAKE_BUILD_TYPE: Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

#SET("OpenCV_DIR" "~/opencv-2.4.9/build/")
#find_package(OpenCV REQUIRED)
#set(OpenCV_FOUND 1)
FIND_PACKAGE(OpenCV REQUIRED)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})


ADD_DEFINITIONS(-Wall -std=c++11)



ADD_EXECUTABLE(lab2 lab2.cpp utils.hpp utils.cpp blockmatching.hpp blockmatching.cpp)
TARGET_LINK_LIBRARIES(lab2 ${OpenCV_LIBS})

ADD_EXECUTABLE(lab3hs lab3hs.cpp utils.hpp utils.cpp HornSchunck.hpp HornSchunck.cpp)
TARGET_LINK_LIBRARIES(lab3hs ${OpenCV_LIBS})

ADD_EXECUTABLE(lab3f lab3f.cpp utils.hpp utils.cpp)
TARGET_LINK_LIBRARIES(lab3f ${OpenCV_LIBS})

INCLUDE_DIRECTORIES(OpticalFlow/mex/)
ADD_EXECUTABLE(lab3mit lab3mit.cpp utils.hpp utils.cpp motionEstimation.cpp motionEstimation.hpp OpticalFlow/mex/OpticalFlow.cpp OpticalFlow/mex/GaussianPyramid.cpp)
TARGET_LINK_LIBRARIES(lab3mit ${OpenCV_LIBS})

ADD_EXECUTABLE(lab3gme lab3gme.cpp GME.hpp GME.cpp utils.hpp utils.cpp motionEstimation.cpp motionEstimation.hpp OpticalFlow/mex/OpticalFlow.cpp OpticalFlow/mex/GaussianPyramid.cpp)
TARGET_LINK_LIBRARIES(lab3gme ${OpenCV_LIBS})
