cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Project naming and configuration
project(wxCAN-Sniffer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(wxBUILD_SHARED OFF)

# Getting wxWidgets
message(STATUS "Fetching wxWidgets...")
include(FetchContent)
FetchContent_Declare(
   wxWidgets
   GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
   #GIT_TAG v3.2.5
   GIT_SHALLOW ON
)
FetchContent_MakeAvailable(wxWidgets)

# Preparing files
set(Sources "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}")
set(SRCS
	${Sources}/Application.h
	${Sources}/Application.cpp
	${Sources}/CANParser.h
	${Sources}/CANParser.cpp
	${Sources}/CircularFrameBuffer.h
	${Sources}/CircularFrameBuffer.cpp
	${Sources}/Common.h
	${Sources}/FormMain.h
	${Sources}/FormMain.cpp
	${Sources}/FramesContainer.h
	${Sources}/FramesContainer.cpp
	${Sources}/Parameters.h
	${Sources}/Parameters.cpp
	${Sources}/resource.h
	${Sources}/ThreadedSerialPort.h
	${Sources}/ThreadedSerialPort.cpp
	${Sources}/${PROJECT_NAME}.rc
	${Sources}/CAN.ico
)

message(STATUS "Project name:      " ${PROJECT_NAME})
message(STATUS "Source files path: " ${Sources})
message(STATUS "wxWidgets sources: " ${wxWidgets_SOURCE_DIR})

# Building
if(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SRCS})
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${Sources}/Info.plist)
else()
    add_executable(${PROJECT_NAME} WIN32 ${SRCS} ${Sources}/${PROJECT_NAME}.manifest)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE wxcore wxnet)
