#!bin/bash

includes="-Ithird_party -Ithird_party/Includes"
lib="-luser32 -lopengl32"
warnings=-Wno-writable-strings

clang++ $includes -g src/main.cpp -oACE.exe $lib $warnings