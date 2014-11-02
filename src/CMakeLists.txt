set(SRC_LIST
  main.cpp
  mainwindow.cpp
  settings_dialog.cpp
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
  ../3rdparty/src/libsvm.cpp
  ../3rdparty/src/libgist.cpp
  ../3rdparty/src/standalone_image.cpp
)

set(HEADER_LIST
  ../include/mainwindow.h
  ../include/settings_dialog.h
  ../include/my_widgets.h
  ../include/flora_ident_app.h
  ../include/common.h
  ../include/core/img_feature.h
  ../include/core/color_hist.h
  ../include/core/fourier_hist.h
  ../include/core/hough_hist.h
  ../include/core/img_descriptor.h
  ../include/core/gist.h
  ../include/core/svm.h  
  ../include/core/knn.h
  ../include/core/flora_ident.h
)

WRAP_UI_FILE(HEADER_UI
  mainwindow.ui
  settings_dialog.ui
)

add_executable(ObjIdent WIN32 ${SRC_LIST} ${HEADER_LIST} ${HEADER_UI})
target_link_libraries(ObjIdent ${Qt_LIBS} ${OpenCV_LIBS} ${FFTWF_LIB} ${TBB_LIBRARIES})
