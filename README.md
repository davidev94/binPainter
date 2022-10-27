# binPainter - VSCODE

A simple tool useful for representing files as images, to better see the structures between bytes. With this tool you can quickly get rough information about the encryption, entropy of a file. I also use this tool to analyze binary files that need to be flashed into embedded systems to verify correct memory usage.

## Getting Started

In the repository you will find .sh/.bat files to build and test the project on macOS/Windows (tested on Monterey and Windows 10).

You will also find some binaries built for the architectures I'm using.

### Prerequisites
You need a C compiler to build the project. This is pretty basic, and if you need help to compile a C file maybe a web search can help you. Here just some notes:

In macOS you can get the standard C compiler using the terminal and typing:
```
xcode-select --install
```
and than following istructions.

In Windows I suggest to download and use *Dev-C++* (Bloodshed/Embarcadero). You can simply open with *Dev-C++* the .c file and compile it or you can search in the *Dev-C++* installation folders for the *gcc* compiler and use it directly (see .bat in the project for examples). Otherwise you can use *MinGW/CygWin*.

### Usage

```
usage: binPainter [option] filename
option:
   -c arg     choose color map, default arg=0
              can be 0,1,2
   -w arg     choose bitmap width, default arg=256
   -s arg     choose image scale, default arg=1
   -p         printf pixel data in hex
   -h         print this help

example: binPainter -w 128 -c 1 testFile.bin
```

## Notes

This is the first project that I publish on *Github*, so I'm using it to learn and it's far to be perfect.
