# ultrasonic2root

Simple ROOT script to convert ultrasonic A-C-S data into a ROOT file.
Further analysis can then be performed.

To run, source your ROOT environment with

$ source /your/root/path/bin/thisroot.sh

and then run

$ root -l main.cc+g

or compile with

$ g++ -I `root-config --incdir` -o ultrasonic.exe main.cc `root-config --libs` -Wall -std=c++0x -pedantic -Wextra

and then run ./ultrasonic.exe

The input file to be used can be passed as an argument.