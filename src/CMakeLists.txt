set(SRC_LIST
  main.cpp
  mainwindow.cpp
  my_widgets.cpp
  flora_ident_app.cpp
  core/color_hist.cpp
  core/fourier_hist.cpp
  core/hough_hist.cpp
  core/img_descriptor.cpp
  core/svm.cpp
  core/knn.cpp
  core/flora_ident.cpp
  3rdparty/libsvm.cpp
  3rdparty/libgist.cpp
  3rdparty/standalone_image.cpp
)
set(HEADER_LIST
  ${MY_INCLUDE_DIR}/mainwindow.h
  ${MY_INCLUDE_DIR}/my_widgets.h
  ${MY_INCLUDE_DIR}/flora_ident_app.h
  ${MY_INCLUDE_DIR}/core/color_hist.h
  ${MY_INCLUDE_DIR}/core/fourier_hist.h
  ${MY_INCLUDE_DIR}/core/hough_hist.h
  ${MY_INCLUDE_DIR}/core/img_descriptor.h
  ${MY_INCLUDE_DIR}/core/svm.h
  ${MY_INCLUDE_DIR}/core/knn.h
  ${MY_INCLUDE_DIR}/core/flora_ident.h
  ${MY_INCLUDE_DIR}/3rdparty/libsvm.h
  ${MY_INCLUDE_DIR}/3rdparty/libgist.h
  ${MY_INCLUDE_DIR}/3rdparty/standalone_image.h
)
WRAP_UI_FILE(HEADER_UI mainwindow.ui)

add_executable(FloraIdent WIN32 ${SRC_LIST} ${HEADER_LIST} ${HEADER_UI})
target_link_libraries(FloraIdent ${Qt_LIBS} ${OpenCV_LIBS} ${FFTWF_LIB})