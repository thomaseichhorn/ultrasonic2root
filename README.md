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


Installing the Olympus NDT Software in a Virtual Machine:

- install Virtualbox
- create a virtual machine running Microsoft Windows
- add your user to the vboxuser group:
    $ sudo adduser $USER vboxusers
- log out and log in again to refresh
- in Windows: install OmniPC and the KeyTool from the USB Stick provided with the device
- connect the USB Key Dongle to your computer, but do not mount it
- in Virtualbox: click Device - USB - select the key
- run the KeyTool
- run OmniPC, right click on the A-C-S image to export
- the exported .txt file is saved in C:\Users\Public\Documents\Olympus NDT\OmniPC\Export
- copy/move the files to your ultrasonic2root directory