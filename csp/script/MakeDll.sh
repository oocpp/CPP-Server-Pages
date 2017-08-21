#!/usr/bin/env bash
g++ ../../work/ROOT_TEMP/$1.cpp -fPIC -shared -o ../../work/ROOT/$1.so
rm *.o -f
