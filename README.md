# ael
A simple experimental script language
##UNIX
###COMPILE INTERPRETER
```bash
g++ -O3 -o ael interpreter.cpp -std=c++0x -ldl
```
MAKE SHARED LIBRARY
```bash
g++ -g -fPIC -c <my_file.cpp>
g++ -g <my_file.o> -shared -Wl,-soname,<file_name.so.1> -o <file_name.so.1> -l<your_dependencies>
```
##WIN32
###COMPILE INTERPRETER (Mingw)
```bash
g++ interpreter.cpp -o ael.exe -O3 -std=c++11
```
###MAKE SHARED LIBRARY (Mingw)
```bash
g++ -c <my_file.cpp> -std=c++11
g++ -shared -o <file_name.dll> <file_name.o> -Wl,-out-implib,<file_name.a>
```
