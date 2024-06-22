#!/bin/bash

echo "Building the project..."
mkdir build
cd build || exit
cmake ..
make
echo "Project built."