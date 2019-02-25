cmake_minimum_required(VERSION 3.9.6)
project(duplicate_file_deleter)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

find_package(Qt5Widgets REQUIRED)
add_executable(${PROJECT_NAME}
    duplicate_file_deleter.cpp
    main_window.h
    main_window.cpp
    progress_dialog.cpp
    progress_dialog.h
    scanner.h
    resources/resources.qrc
)
qt5_use_modules(${PROJECT_NAME} Widgets)
