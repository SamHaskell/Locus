#!/bin/bash

echo "Building the project..."
mkdir Build
cd Build || exit
cmake ..
make
echo "Project built."