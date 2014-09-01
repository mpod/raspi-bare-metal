#include "raycasting.h"
#include "qemu.h"
#include "int.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

uint16_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

void pl011_uart_write_char(char value)
{
	volatile uint32_t* paddr = (volatile uint32_t*)(PL011_UART_BASE + PL011_UARTFR);

	/* Wait until the serial buffer is empty */
	while (*paddr & PL011_UARTFR_TXFF);

	/* Put value into the serial buffer */
	paddr = (volatile uint32_t*)(PL011_UART_BASE + PL011_UARTDR);
	*paddr = value;
}

void pl011_uart_write_string(const char * str)
{
	while (*str) pl011_uart_write_char(*str++);
}

void pl011_uart_write_hex(uint32_t value)
{
	char* hex = "0123456789ABCDEF";

	pl011_uart_write_char(*(hex + ((value >> 12) & 0xF)));
	pl011_uart_write_char(*(hex + ((value >> 8) & 0xF)));
	pl011_uart_write_char(*(hex + ((value >> 4) & 0xF)));
	pl011_uart_write_char(*(hex + (value & 0xF)));
	pl011_uart_write_char(' ');
}

void pl050_init(void)
{
	volatile uint32_t* paddr = (volatile uint32_t*)(PL050_KMI_KB_BASE + PL050_KMICR);
	*paddr = PL050_KMICR_KMIEN; // Enable keyboard
}

void pl110_init(void)
{
	volatile uint32_t* paddr;
	paddr = (volatile uint32_t*)(PL110_BASE + PL110_LCDTIMING0);
	/* 640x480 pixels */
	*paddr = 0x3f1f3f9c;
	paddr = (volatile uint32_t*)(PL110_BASE + PL110_LCDTIMING1);
	*paddr = 0x080b61df;
	paddr = (volatile uint32_t*)(PL110_BASE + PL110_LCDUPBASE);
	*paddr = (uint32_t)framebuffer;
	/* 16-bit color */
	paddr = (volatile uint32_t*)(PL110_BASE + PL110_LCDCONTROL);
	*paddr = 0x1829;
}

void draw_vert_line(int x, int y, int h, int color_mode)
{
	if (x >= SCREEN_WIDTH) x = SCREEN_WIDTH - 1;
	if (x < 0) x = 0;
	if (y >= SCREEN_HEIGHT) y = SCREEN_HEIGHT - 1;
	if (y < 0) y = 0;
	if (h < 0) h = 0;
	if (y + h > SCREEN_HEIGHT) h = SCREEN_HEIGHT - y;

	uint32_t i;

	if (color_mode) {
		for (i = y; i < y + h; i++)
			framebuffer[i * SCREEN_WIDTH + x] = 0x3f << (5);
	} else {
		for (i = y; i < y + h; i++)
			framebuffer[i * SCREEN_WIDTH + x] = 0x0;
	}
}

void update_display(void)
{
}

int run(void)
{
	pl050_init();
	pl110_init();

	init_world(SCREEN_WIDTH, SCREEN_HEIGHT);
	cast_rays(draw_vert_line, update_display);

	volatile uint32_t* paddr1 = (volatile uint32_t*)(PL050_KMI_KB_BASE + PL050_KMISTAT);
	volatile uint32_t* paddr2 = (volatile uint32_t*)(PL050_KMI_KB_BASE + PL050_KMIDATA);
	uint32_t key;
	while (1) {
		while (*paddr1 & PL050_KMISTAT_RXFULL) {
			key = *paddr2;

			pl011_uart_write_string("Key 0x");
			pl011_uart_write_hex(key);
			pl011_uart_write_char('\n');

			switch (key & 0xFF) {
				case 0x11: // W
					player.move_dir = 1.;
					break;
				case 0x1F: // S
					player.move_dir = -1.;
					break;
				case 0x1E: // A
					player.rot_dir = 1.;
					break;
				case 0x20: // D
					player.rot_dir = -1.;
					break;
				default:
					break;
			}
			move_player();
			player.move_dir = 0;
			player.rot_dir = 0;
			cast_rays(draw_vert_line, update_display);
		}
	}
	return 0;
}
