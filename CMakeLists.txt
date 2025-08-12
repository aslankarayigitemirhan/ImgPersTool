cmake_minimum_required(VERSION 3.16)

project(internship VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/eigen)

set(PROJECT_SOURCES
    src/MainWindow.cpp
    src/corner_detector.cpp
    src/homography.cpp
    src/main.cpp
    include/MainWindow.h
    include/corner_detector.hpp
    include/homography.hpp
    include/util.hpp

)

add_executable(internship ${PROJECT_SOURCES})

target_link_libraries(internship Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    ${OpenCV_LIBS})
