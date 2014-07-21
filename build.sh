module load gcc
module load qt
module load opencv
cd build
cmake -D USE_QT4=ON ../
make