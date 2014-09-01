all: pitft

sdl:
	make -f sdl.make

qemu:
	make -f raspi.make qemu

runqemu:
	make -f raspi.make runqemu

pitft:
	make -f raspi.make pitft

