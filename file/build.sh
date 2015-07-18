#!/usr/bin/bash
echo compile..
g++ -g -fPIC -c file.cpp -I ../poo -I .. -std=c++0x -O3
echo link
g++ -g file.o -shared -Wl,-soname,file.so.1 -o file.so.1
echo ok
echo running test
echo -------------------------------------------------
../ael ./file.ael
echo -------------------------------------------------