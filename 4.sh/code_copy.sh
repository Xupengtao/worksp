#!/bin/sh
cd $1
mkdir WorkSp
cd WorkSp
cp -r ~/WorkSp/2.py .
cp -r ~/WorkSp/3.cpp .
cp -r ~/WorkSp/4.sh .
cd 2.py
rm Analysis
rm Data
rm Docs
rm Source
cd ..
cd 3.cpp/C++
rm Analysis
rm Data
rm Docs
rm Source
cd Projects
rm Tools
cd ..
cd ..
cd CUDA
rm Analysis
rm Data
rm Docs
rm Source
cd Projects
rm Tools