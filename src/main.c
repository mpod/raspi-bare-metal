#if defined(SDL)
#include "sdl.h"
#elif defined(QEMU)
#include "qemu.h"
#elif defined(PITFT)
#include "pitft.h"
#elif defined(FB)
#include "fb.h"
#else 
#include "fb.h"
#endif

int main(void)
{
	return run();
}
