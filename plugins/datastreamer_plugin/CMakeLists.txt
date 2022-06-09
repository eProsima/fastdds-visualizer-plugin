# Copyright 2022 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# This file is part of eProsima Fast DDS Monitor.
#
# eProsima Fast DDS Monitor is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# eProsima Fast DDS Monitor is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with eProsima Fast DDS Monitor. If not, see <https://www.gnu.org/licenses/>.

###############################################################################
# Configure CMake values
###############################################################################

set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


###############################################################################
# Project
###############################################################################
project(plotjuggler_fastdds_datastream_plugin)


###############################################################################
# Load external projects
###############################################################################
# Load eProsima packages
find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)

# Load Qt requirements
find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
    Concurrent
    Xml
    Svg
    WebSockets)


###############################################################################
# Include Qt
###############################################################################
# include_directories(
#     ${Qt5Core_INCLUDE_DIRS}
#     ${Qt5Widgets_INCLUDE_DIRS}
#     ${Qt5Concurrent_INCLUDE_DIRS}
#     ${Qt5Xml_INCLUDE_DIRS}
#     ${Qt5WebSockets_INCLUDE_DIRS}
#     ${Qt5Svg_INCLUDE_DIRS}
# )

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
    ${Qt5Svg_INCLUDE_DIRS} )

set( QT_LINK_LIBRARIES
    Qt5::Core
    Qt5::Widgets
    Qt5::Concurrent
    Qt5::Xml
    Qt5::Svg
    Qt5::WebSockets
    )


###############################################################################
# Configure Qt values
###############################################################################

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)

# TODO: add it when UI is implemented
# SET(
#     QT_UI_SOURCES
#         "ui/topic_selection_dialog/dialogselecttopics.ui")

# QT5_WRAP_UI (
#         COMMON_UI_SRC
#         "${UI_SOURCES}"
#     )


###############################################################################
# Getting sources
###############################################################################

set (ALL_LIBRARY_SOURCES

        # DataStreamer files
        "datastreamer/FastDdsDataStreamer.cpp"
        "datastreamer/FastDdsDataStreamer.hpp"
   )


###############################################################################
# Compile
###############################################################################

# Include PlogJuggler directories
find_package(plotjuggler REQUIRED)
message(STATUS "PlotJuggler FOUND")
message(STATUS "plotjuggler_INCLUDE_DIR: ${plotjuggler_INCLUDE_DIR}")
message(STATUS "plotjuggler_LIBRARIES: ${plotjuggler_LIBRARIES}")

include_directories(${plotjuggler_INCLUDE_DIR})

# Create library
add_library(
    ${PROJECT_NAME}
    SHARED
        ${ALL_LIBRARY_SOURCES}
    )

# Link required libraries
target_link_libraries(
    ${PROJECT_NAME}
        ${QT_LINK_LIBRARIES}
        ${plotjuggler_LIBRARIES}
    )


###############################################################################
# Install
###############################################################################

install(
    TARGETS
        ${PROJECT_NAME}
    DESTINATION
        ${BIN_INSTALL_DIR}  )


###############################################################################
# Disable QDebug traces in Release
###############################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<NOT:$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">>:QT_NO_DEBUG_OUTPUT>)
