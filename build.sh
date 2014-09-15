module load gcc
module load opencv
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_QT4=ON -DOpenCV_DIR=/l/opencv-2.4.9/share/OpenCV/ ..
make -j8