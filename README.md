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

    $ sudo apt-get install libpixman-1-dev libpixman-1-0 libsdl-image1.2-dev

Download and extract the latest stable version of QEMU from [http://wiki.qemu.org/Download](http://wiki.qemu.org/Download).

    $ wget http://wiki.qemu-project.org/download/qemu-2.1.0.tar.bz2
    $ tar xvjf qemu-2.1.0.tar.bz2

Configure, build, install, and verify qemu-system-arm.

    $ cd qemu-2.1.0
    $ ./configure --target-list=arm-softmmu,arm-linux-user --enable-sdl
    $ make -j 2
    $ sudo make install
    $ qemu-system-arm --version

### Building QEMU kernel image ###

    $ cd raspi-bare-metal
    $ make qemu
    $ make runqemu
  
Moving controls: W, A, S, D.

## Framebuffer Raspberry Pi kernel image ##

Build kernel image file. Make sure to have properly configured ARM toolchain.

    $ cd raspi-bare-metal
    $ make fb

Get a Raspberry PI SD card which has an operating system installed already. On SD card find file kernel.img and rename it to something else, such as kernel.img.original. Then, copy generated file kernel.img onto the SD card. Put the SD card into a Raspberry Pi and turn it on. For left, up, down, and right moving controls are used GPIO ports 23, 22, 21, and 18, respectively. 

## PiTFT Raspberry Pi kernel image ##

This image uses [Adafruit PiTFT](http://www.adafruit.com/product/1601). 

    $ cd raspi-bare-metal
    $ make pitft

Copy file kernel.img onto the SD card. Plug PiTFT into your Raspberry Pi, insert SD card, and turn Raspberry Pi on. For left, up, down, and right moving controls are used GPIO ports 23, 22, 21, and 18, respectively. 

## Debugging ##

### Debugging QEMU session ###

One way is to use GDB. Run QEMU kernel image with following parameters.

    $ qemu-system-arm -m 128 -kernel kernel.elf -serial stdio -machine integratorcp -S -s

In another console window start remote GDB session.

    $ arm-none-eabi-gdb kernel.elf
    GNU gdb (GNU Tools for ARM Embedded Processors) 7.6.0.20131129-cvs
    Copyright (C) 2013 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "--host=i686-linux-gnu --target=arm-none-eabi".
    For bug reporting instructions, please see:
    <http://www.gnu.org/software/gdb/bugs/>...
    Reading symbols from /raspi-bare-metal/kernel.elf...(no debugging symbols found)...done.
    (gdb) target remote localhost:1234
    Remote debugging using localhost:1234
    0x00008010 in pl011_uart_write_char ()
    (gdb)

Another way is use of implemented UART functions that print data to stdio.

    // Send string to stdio
    void pl011_uart_write_string(const char * str);

    // Send hex value to stdio
    void pl011_uart_write_hex(uint32_t value);

### Debugging Rasperry Pi ###

Debugging of bare metal image on Raspberry Pi is not an easy task. The simplest method is use of OK LED for detection of interesting events. Another way is use of UART peripherals of Raspberry Pi. To connect Raspberry Pi and PC you will need [console cable](https://learn.adafruit.com/adafruits-raspberry-pi-lesson-5-using-a-console-cable). Following C functions are implemented for sending characters to UART peripherals.
 
    // Initialize mini UART 
    void bcm2835_aux_muart_init(void);

    // Send string over mini UART
    void bcm2835_aux_muart_transfernb(char* tbuf);

    // Send hex value over mini UART
    void bcm2835_aux_muart_transfer_hex(uint32_t value);

## References ##

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


