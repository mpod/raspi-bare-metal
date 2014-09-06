raspi-bare-metal
================

Purpose of this project is to get some experience with bare metal programming for Raspberry Pi and QEMU ARM machine. Raycasting algorihm is chosen as an interesting and simple showcase example. Project produces four types of outputs: 

* reference implementation using SDL library, 
* QEMU kernel image,
* Raspberry Pi kernel image that uses framebuffer, 
* and Raspberry Pi kernel image that draws on Adafruit PiTFT display.

## SDL reference implementation ##

    $ sudo apt-get install libsdl2-2.0-0 libsdl2-dev
    $ make sdl
    $ ./game

## QEMU kernel image ##

### ARM toolchain ###

Download and extract ARM toolchain package from [https://launchpad.net/gcc-arm-embedded/](https://launchpad.net/gcc-arm-embedded/). Add bin directory of extracted package to the PATH environment variable.

### QEMU system ###

Install prerequisite packages.

    $ sudo apt-get install libpixman-1-dev libpixman-1-0

Download and extract the latest stable version of QEMU from [http://wiki.qemu.org/Download](http://wiki.qemu.org/Download).

    $ wget http://wiki.qemu-project.org/download/qemu-2.1.0.tar.bz2
    $ tar xvjf qemu-2.1.0.tar.bz2

Configure, build, install, and verify qemu-system-arm.

    $ cd qemu-2.1.0
    $ ./configure --target-list=arm-softmmu,arm-linux-user
    $ make -j 2
    $ sudo make install
    $ qemu-system-arm --version

### Building QEMU kernel image ###

    $ cd raspi-bare-metal
    $ make qemu
    $ make runqemu
  

Framebuffer Raspberry Pi kernel image
-------------------------------------

Build kernel image file. Make sure to have properly configured ARM toolchain.

    $ cd raspi-bare-metal
    $ make fb

Get a Raspberry PI SD card which has an operating system installed already. On SD card find file kernel.img and rename it to something else, such as kernel.img.original. Then, copy generated file kernel.img onto the SD card. Put the SD card into a Raspberry Pi and turn it on.


PiTFT Raspberry Pi kernel image
-------------------------------

This image uses [Adafruit PiTFT](http://www.adafruit.com/product/1601). 

    $ cd raspi-bare-metal
    $ make pitft

Copy file kernel.img onto the SD card. Plug PiTFT into your Raspberry Pi, insert SD card, and turn it on.

References
----------

This project is a compilation of knowledge from various places over Internet.

* [SDL Documentation](http://aaaa/)
* [ARM PrimeCell PS2 Keyboard/Mouse Interface (PL050)](http://aaaa)
* [PrimeCell UART (PL011)](http://aaaa)
* [PrimeCell Color LCD Controller (PL110)](http://aaaa)
* [ILITEK ILI9341](http://aaaa)


