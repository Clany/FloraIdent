set(SRC_LIST
  main.cpp
  mainwindow.cpp
  my_widgets.cpp
  flora_ident_app.cpp
  core/color_hist.cpp
  core/fourier_hist.cpp
  core/hough_hist.cpp
  core/img_descriptor.cpp
  core/libsvm.cpp
  core/svm.cpp
  core/knn.cpp
  core/flora_ident.cpp
)
set(HEADER_LIST
  ${PROJ_INCLUDE}/mainwindow.h
  ${PROJ_INCLUDE}/my_widgets.h
)
WRAP_UI_FILE(HEADER_UI mainwindow.ui)

add_executable(FloraIdent WIN32 ${SRC_LIST} ${HEADER_LIST} ${HEADER_UI})
target_link_libraries(FloraIdent ${Qt_LIBS} ${OpenCV_LIBS})