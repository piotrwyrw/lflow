#!/bin/bash

# Just a quick script that moves only the necessary source files to a WSL directory
# and compiles the project. Use with 'source'.

echo "Preparing environment ..."

if [ ! -d wsl ]; then
	mkdir wsl
else
	rm -rf wsl
	mkdir wsl
fi

cp -rf src/ main.c CMakeLists.txt wsl/
cp cmake-build-debug/main.flow wsl/
cd wsl/

echo "Building ..."

cmake .
make

echo "Done."

