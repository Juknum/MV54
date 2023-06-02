project(RA_project)
cmake_minimum_required(VERSION 3.0)

set(VISP_STATIC TRUE)
find_package(OpenCV  REQUIRED)
find_package(CURL REQUIRED)
find_package(VISP REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIR})
include_directories(${VISP_INCLUDE_DIRS})

add_executable(RA_project unityInterface.h unityInterface.cpp main.cpp)

target_link_libraries(RA_project
        ${OpenCV_LIBS}
        ${CURL_LIBRARIES}
        ${VISP_LIBRARIES}
)