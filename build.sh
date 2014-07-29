module load gcc
module load qt
module load opencv
mkdir build
cd build
cmake -D USE_QT4=ON -D OpenCV_DIR=/l/opencv-2.4.9/share/OpenCV/ ..
make