cmake_minimum_required(VERSION 3.30)

project(HWPXViewer)

set(CMAKE_CXX_STANDARD 11)

find_package(Qt5Widgets REQUIRED)
find_package(LibXml2 REQUIRED)
find_library(LIBZIP_LIB zip)

add_executable(hwpx_viewer main.cpp)

target_link_libraries(hwpx_viewer Qt5::Widgets ${LIBXML2_LIBRARIES} ${LIBZIP_LIB})
include_directories(${LIBXML2_INCLUDE_DIRS})
