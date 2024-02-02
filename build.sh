#!bin/bash

lib=-luser32
warnings=-Wno-writable-strings

clang -g src/main.cpp -oACE.exe $lib $warnings