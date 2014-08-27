set(SRC_LIST
  main.cpp
  mainwindow.cpp
  my_widgets.cpp
  flora_ident_app.cpp
  core/color_hist.cpp
  core/fourier_hist.cpp
  core/hough_hist.cpp
  core/img_descriptor.cpp
  core/gist.cpp
  core/svm.cpp
  core/knn.cpp
  core/flora_ident.cpp
  3rdparty/libsvm.cpp
  3rdparty/libgist.cpp
  3rdparty/standalone_image.cpp
)
set(HEADER_LIST
  ${PROJ_INCLUDE_DIR}/mainwindow.h
  ${PROJ_INCLUDE_DIR}/my_widgets.h
  ${PROJ_INCLUDE_DIR}/flora_ident_app.h
  ${PROJ_INCLUDE_DIR}/common.h
  ${PROJ_INCLUDE_DIR}/core/color_hist.h
  ${PROJ_INCLUDE_DIR}/core/fourier_hist.h
  ${PROJ_INCLUDE_DIR}/core/hough_hist.h
  ${PROJ_INCLUDE_DIR}/core/img_descriptor.h
  ${PROJ_INCLUDE_DIR}/core/gist.h
  ${PROJ_INCLUDE_DIR}/core/svm.h  
  ${PROJ_INCLUDE_DIR}/core/knn.h
  ${PROJ_INCLUDE_DIR}/core/flora_ident.h
  ${PROJ_INCLUDE_DIR}/3rdparty/libsvm.h
  ${PROJ_INCLUDE_DIR}/3rdparty/libgist.h
  ${PROJ_INCLUDE_DIR}/3rdparty/standalone_image.h
)
WRAP_UI_FILE(HEADER_UI mainwindow.ui)

add_executable(FloraIdent WIN32 ${SRC_LIST} ${HEADER_LIST} ${HEADER_UI})
target_link_libraries(FloraIdent ${Qt_LIBS} ${OpenCV_LIBS} ${FFTWF_LIB})
