#!/bin/bash
cmake -DCMAKE_BUILD_TYPE=Debug .
cmake --build . --target hash

if [ $? -eq 0 ]; then
  sudo chown root:root hash
  sudo chmod u+s hash

  if [ $? -eq 0 ]; then
    rm -r CMakeFiles/
    rm cmake_install.cmake
    rm CMakeCache.txt
  fi
fi