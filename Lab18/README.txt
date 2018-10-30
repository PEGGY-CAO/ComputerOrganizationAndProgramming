======================================================================
CS2110                        Lab 18                      Fall 2018
======================================================================
Read the instructions in the pixels.c file

Dependencies
------------

If you are using Docker, simply re-run the script to update the image.

Otherwise (for native or virtual machines):

	$ sudo apt update
	$ sudo apt install gcc-arm-none-eabi cs2110-vbam-sdl cs2110-gba-linker-script nin10kit

(If you don't have it already from installing complx, you may need to
 add Brandon's 2110 package repository with

    $ sudo add-apt-repository ppa:tricksterguy87/ppa-gt-cs2110
 
 and then re-run the two commands above.)

cd into directory containing pixels.c and Makefile

Assignment
----------

Read the instructions in the pixels.c file.

To build and test:

 1. cd into directory containing pixels.c and Makefile
 2. Compile it with 'make'
 3. Run it with 'make vba'
 4. Check to make sure your output is identical to lab18.png

When you are done, come show it to us
