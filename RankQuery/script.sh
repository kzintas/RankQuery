#! /bin/bash
#g++ -o rank2 Source.cpp #-i headers -I folders 
g++ -std=c++11 Source.cpp Wavelet/wavelet.cpp Select/Select.cpp Rank/Rank.cpp -o src -D_GLIBCXX_USE_CXX11_ABI=1
./src $1 $2 $3
#echo "$#"
