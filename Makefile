all: fb

sdl:
	@echo "*** Preparing SDL executable. ***"
	make -f sdl.make
	@echo "*** Run ./game ***"

qemu:
	@echo "*** Preparing QEMU kernel image. ***"
	make -f raspi.make qemu
	@echo "*** Run make runqemu ***"

runqemu:
	make -f raspi.make runqemu

pitft:
	@echo "*** Preparing image for Adafruit TFT panel. ***"
	make -f raspi.make pitft
	@echo "*** Copy kernel.img to SD card. ***"

fb:
	@echo "*** Preparing image for Raspberry Pi framebuffer. ***"
	make -f raspi.make fb
	@echo "*** Copy kernel.img to SD card. ***"
