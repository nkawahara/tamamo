#/bin/bash

wget https://pocoproject.org/releases/poco-1.7.7/poco-1.7.7.tar.gz
tar xvf poco-1.7.7.tar.gz
cd poco-1.7.7
echo "IMPORTANT: Make sure that the path to the build directory does not contain symbolic links. Furthermore, on Mac OS X (or other systems with case insensitive filesystems), make sure that the characters in the path have the correct case. Otherwise you'll get an error saying Current working directory not under $PROJECT_BASE.."
./configure --omit=Data/ODBC,Data/MySQL --prefix=$HOME/usr  --no-tests --no-samples
make -j4
make install 
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/usr/lib
#export PATH=$PATH:$HOME/usr/bin
#g++ -I ~/usr/include/ -L ~/usr/lib/  -l PocoFoundationd test.cpp
