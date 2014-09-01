#if defined(SDL)
#include "sdl.h"
#elif defined(QEMU)
#include "qemu.h"
#elif defined(PITFT)
#include "pitft.h"
#else 
#include "pitft.h"
#endif

int main(void)
{
	return run();
}
