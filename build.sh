module load gcc
module load opencv
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release -DUSE_QT4=ON -DOpenCV_DIR=/l/opencv-2.4.9/share/OpenCV/ -DCMAKE_CXX_FLAGS="-I/l/gcc-4.7.2/include/c++/4.7.2/x86_64-unknown-linux-gnu -I/l/gcc-4.7.2/include/c++/4.7.2" -DCMAKE_EXE_LINKER_FLAGS="-L/l/gcc-4.7.2/lib64" ..
make -j8