cmake_minimum_required(VERSION 3.16)

project(clip2path
    VERSION 0.1.0
    DESCRIPTION "Clipboard Image To Path"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.hpp
)

qt_add_executable(clip2path ${SOURCES})

target_include_directories(clip2path PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(clip2path PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

install(TARGETS clip2path DESTINATION ${CMAKE_INSTALL_BINDIR})
