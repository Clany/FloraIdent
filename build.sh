module load gcc
module load qt
cd build
cmake -D USE_QT4=ON ../
make