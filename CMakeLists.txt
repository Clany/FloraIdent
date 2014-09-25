cmake_minimum_required (VERSION 2.8.11)
project(FloraIdent)

list(APPEND CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
  $ENV{DEV_LIBS}/clany/cmake
)
include(Utilities)

# Compiler detection
find_compiler(
  GNU   4.7  "-std=c++11 -Wall -fopenmp -fno-operator-names"
  Clang 3.2  "-std=c++11 -Wall -fopenmp -fno-operator-names"
  MSVC  2013 "/W4 /openmp"
)

option(USE_MY_PATH "Use my own default library path" OFF)
if(USE_MY_PATH)
  add_default_lib_path($ENV{DEV_LIBS} $ENV{DEV_PLAT})
endif()

# Find Qt library
option(USE_QT4 OFF)

if(USE_QT4)
  find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
  include(${QT_USE_FILE})
  set(Qt_LIBS ${QT_LIBRARIES})
  macro(WRAP_UI_FILE files)
    QT4_WRAP_UI(${ARGV})
  endmacro()
  set(USE_QT_VER 4)
else()
  find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
  set(Qt_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
  macro(WRAP_UI_FILE files)
    QT5_WRAP_UI(${ARGV})
  endmacro()
  set(USE_QT_VER 5)
endif()

#set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up project include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/include)
include_directories(${PROJECT_BINARY_DIR})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(FFTW REQUIRED)
include_directories(${FFTW_INCLUDES})

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

configure_file (
  "${PROJECT_SOURCE_DIR}/include/flora_ident_config.h.in"
  "${PROJECT_BINARY_DIR}/flora_ident_config.h"
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(src)
