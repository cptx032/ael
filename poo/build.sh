#!/usr/bin/bash
echo compile..
g++ -g -fPIC -c poo.cpp -I .. -std=c++0x -O3 -DAEL_LIB
echo link
g++ -g poo.o -shared -Wl,-soname,poo.so.1 -o poo.so.1
echo ok
echo running test
echo -------------------------------------------------
../ael ./poo.ael
echo -------------------------------------------------