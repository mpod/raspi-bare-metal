raspi-bare-metal
================

Purpose of this project is to get some experience with bare metal programming for Raspberry Pi and QEMU ARM machine. Raycasting algorihm is chosen as an interesting and simple showcase example. Project produces four types of outputs: 

* reference implementation using SDL library, 
* QEMU kernel image,
* Raspberry Pi kernel image that uses framebuffer, 
* and Raspberry Pi kernel image that draws on Adafruit PiTFT display.

<img src="https://raw.github.com/mpod/raspi-bare-metal/master/pitft.png"/>

## SDL reference implementation ##

Install prerequisite packages, build, and run executable.

    $ sudo apt-get install libsdl2-2.0-0 libsdl2-dev
    $ make sdl
    $ ./game

Moving controls: W, A, S, D.

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
  
Moving controls: W, A, S, D.

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

* [Ray-Casting Tutorial](http://www.permadi.com/tutorial/raycast/index.html)
* [SDL Documentation](http://wiki.libsdl.org/CategoryAPI)
* [QEMU Emulator User Documentation](http://wiki.qemu.org/download/qemu-doc.html)
* [ARM PrimeCell PS2 Keyboard/Mouse Interface (PL050) - Technical Reference](http://infocenter.arm.com/help/topic/com.arm.doc.ddi0143c/DDI0143.pdf)
* [PrimeCell UART (PL011) - Technical Reference](http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183g/DDI0183G_uart_pl011_r1p5_trm.pdf)
* [PrimeCell Color LCD Controller (PL110) - Technical Reference](http://infocenter.arm.com/help/topic/com.arm.doc.ddi0161e/DDI0161.pdf)
* [QEMU source code](https://github.com/qemu/qemu)
* [BCM2835 ARM Peripherals - Technical Reference](http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf)
* [C library for Broadcom BCM 2835 as used in Raspberry Pi](http://www.airspayce.com/mikem/bcm2835/)
* [Baking Pi – Operating Systems Development](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/)
* [RPi Low-level peripherals](http://elinux.org/RPi_Low-level_peripherals)
* [RPi Framebuffer](http://elinux.org/RPi_Framebuffer)
* [RPi SPI](http://elinux.org/RPi_SPI)
* [ILITEK ILI9341 - Technical Reference](http://www.displayfuture.com/Display/datasheet/controller/ILI9341.pdf)
* [Linux Framebuffer drivers for small TFT LCD display modules](https://github.com/notro/fbtft)
* [Library for the Adafruit 2.2" SPI display](https://github.com/adafruit/Adafruit_ILI9340)


