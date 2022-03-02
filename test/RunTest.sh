#!bin/zsh

cd ..

build/./binPainter -s 20 -w 16 test/file1.bin
build/./binPainter -s 2 -w 256 -c 1 test/file2.bin
