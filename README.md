Console-variant-of-Win32DiskImager
win32 console app for  writing image on flash drive

---------------------
How to build
---------------------
Using MInGW

g++.exe main.cpp imager.cpp disk.cpp -o imager

---------------------
How to use
---------------------
The command format:      imager [flash_drive_name] [path_to_image]

The example of command:  imager d c:/folder1/file1.img"
