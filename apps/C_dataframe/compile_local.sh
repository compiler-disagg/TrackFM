#!/bin/bash
cp make_O3 Makefile
make clean
make -j
clang -O3 nyc.c -I include/ -o nyc -lm libdataframe.so
