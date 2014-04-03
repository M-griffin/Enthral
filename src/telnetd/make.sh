#/bin/bash 
g++ -o entelnetd entelnetd.cpp config.cpp -lutil -std=c++0x -lboost_system

echo done.

