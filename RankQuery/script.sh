#! /bin/bash
g++ -o rank2 Source.cpp 
./rank2 $1 $2 $3
echo "$#"
