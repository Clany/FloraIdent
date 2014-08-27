cmake_minimum_required (VERSION 2.8.11)
project(FloraIdent)

set (FloraIdent_VERSION_MAJOR 1)
set (FloraIdent_VERSION_MINOR 0)

# Compiler detection
if(MSVC)
  if(MSVC_VERSION LESS 1800)
    message(FATAL_ERROR "MSVC version need to be at least 2013")
  endif()
  set(CMAKE_CXX_FLAGS "/openmp")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7)
    message(FATAL_ERROR "GCC version need to be at least 4.7")
  endif()
  set(CMAKE_CXX_FLAGS "-std=c++11 -fopenmp -O2 -fno-operator-names")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "CLANG")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
    message(FATAL_ERROR "Clang version need to be at least 3.2")
  endif()
  set(CMAKE_CXX_FLAGS "-std=c++11 -fopenmp -O2 -fno-operator-names")
else()
  message(FATAL_ERROR "No supported compiler")
endif()

# Find Qt library
option(USE_QT4 OFF)

if(USE_QT4)
  find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
  include(${QT_USE_FILE})
  set(Qt_LIBS ${QT_LIBRARIES})
  macro(WRAP_UI_FILE header_ui ui_file)
    QT4_WRAP_UI(${header_ui} ${ui_file})
  endmacro()
  set(USE_QT_VER 4)
else()
  find_package(Qt5Core REQUIRED)
  find_package(Qt5Gui REQUIRED)
  find_package(Qt5Widgets REQUIRED)
  set(Qt_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
  macro(WRAP_UI_FILE header_ui ui_file)
    QT5_WRAP_UI(${header_ui} ${ui_file})
  endmacro()
  set(USE_QT_VER 5)
endif()

#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find OpenCV and FFTW
set(DEFAULT_LIB_PATH $ENV{DEV_LIBS})

find_package(OpenCV REQUIRED PATHS ${DEFAULT_LIB_PATH}/OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(FFTW REQUIRED PATHS ${DEFAULT_LIB_PATH}/FFTW)
include_directories(${FFTW_INCLUDES})

# Set up project include directories
set(PROJ_INCLUDE ${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJ_INCLUDE})
include_directories(${PROJ_INCLUDE}/3rdparty)
include_directories(${PROJECT_BINARY_DIR})

configure_file (
  "${PROJ_INCLUDE}/flora_ident_config.h.in"
  "${PROJECT_BINARY_DIR}/flora_ident_config.h"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(src)
