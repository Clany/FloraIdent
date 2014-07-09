cmake_minimum_required (VERSION 2.8.11)
project(FloraIdent)

set (FloraIdent_VERSION_MAJOR 1)
set (FloraIdent_VERSION_MINOR 0)

if(MSVC)
  if(MSVC_VERSION LESS 1800)
    message(FATAL_ERROR "MSVC version need to be at least 2013")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7)
    message(FATAL_ERROR "GCC version need to be at least 4.7")
  endif()
  set(CMAKE_CXX_FLAGS "-std=c++11 -fopenmp -O2")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "CLANG")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
    message(FATAL_ERROR "Clang version need to be at least 3.2")
  endif()
  set(CMAKE_CXX_FLAGS "-std=c++11 -fopenmp -O2")
else()
  message(FATAL_ERROR "No supported compiler")
endif()

option(USE_QT4 OFF)

if(USE_QT4)
  find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
  include(${QT_USE_FILE})
  set(QT_LIBS ${QT_LIBRARIES})
  macro(WRAP_UI_FILE header_ui ui_file)
    QT4_WRAP_UI(${header_ui} ${ui_file})
  endmacro()
  set(USE_QT_VER 4)
else()
  find_package(Qt5Core REQUIRED)
  find_package(Qt5Gui REQUIRED)
  find_package(Qt5Widgets REQUIRED)
  set(QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
  macro(WRAP_UI_FILE header_ui ui_file)
    QT5_WRAP_UI(${header_ui} ${ui_file})
  endmacro()
  set(USE_QT_VER 5)
endif()

#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#find_package(OpenCV REQUIRED)

set(MY_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
include_directories(${MY_INCLUDE_DIR})
include_directories(${PROJECT_BINARY_DIR})

configure_file (
  "${MY_INCLUDE_DIR}/flora_ident_config.h.in"
  "${PROJECT_BINARY_DIR}/flora_ident_config.h"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(src)
