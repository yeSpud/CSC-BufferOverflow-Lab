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
    echo -e "\e[1;31m Be sure to leave only the hash program and .shellcode file for the guest user. \e[0m"
    echo -e "\e[1;32m Good luck, and happy hacking! c: \e[0m"
  fi
fi