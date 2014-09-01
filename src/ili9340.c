#include <stdarg.h>
#include "bcm2835.h"
#include "ili9340.h"

uint16_t width;
uint16_t height;
uint8_t rotation;

char framebuffer[2 * ILI9340_TFTWIDTH * ILI9340_TFTHEIGHT];
uint16_t dirty_x0;
uint16_t dirty_y0;
uint16_t dirty_x1;
uint16_t dirty_y1;

void ili9340_write_command(uint8_t command, int param_len, ...)
{
	char buffer[50];
	va_list args;

	bcm2835_gpio_write(25, LOW);
	bcm2835_spi_transfer(command);
	bcm2835_gpio_write(25, HIGH);

	if (param_len) {
		va_start(args, param_len);
		for (uint32_t i = 0; i < param_len; i++) {
			buffer[i] = (uint8_t)va_arg(args, int);
		}
		va_end(args);
		bcm2835_spi_writenb(buffer, param_len);
	}
}

void ili9340_draw_line(void)
{
}

void ili9340_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if (x0 >= width) x0 = width - 1;
	if (y0 >= height) y0 = height - 1;
	if (x1 >= width) x1 = width - 1;
	if (y1 >= height) y1 = height - 1;
	if (x0 > x1 || y0 > y1) return;

	ili9340_write_command(ILI9340_CASET, 4, x0 >> 8, x0 & 0xff, x1 >> 8, x1 & 0xff);
	ili9340_write_command(ILI9340_PASET, 4, y0 >> 8, y0 & 0xff, y1 >> 8, y1 & 0xff);

	ili9340_write_command(ILI9340_RAMWR, 0);
}

void ili9340_draw_pixel(uint16_t x, uint16_t y, uint16_t color) 
{
	if (x >= width) x = width - 1;
	if (y >= height) y = height - 1;

	ili9340_mkdirty(x, y, x, y);

	uint32_t offset = (y * width + x) << 1;
	framebuffer[offset++] = (color >> 8) & 0xff;
	framebuffer[offset] = color & 0xff;
	//bcm2835_aux_muart_transfer_hex(x);
	bcm2835_aux_muart_transfer_hex(y);
	//bcm2835_aux_muart_transfer_hex(offset);
	//bcm2835_aux_muart_transfer('|');
}

void ili9340_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	if (x >= width) x = width - 1;
	if (y >= height) y = height - 1;
	if (x + w > width) w = width - x;
	if (y + h > height) h = height - y;

	ili9340_mkdirty(x, y, x + w - 1, y + h - 1);

	uint8_t hi = (color >> 8) & 0xff, lo = color & 0xff;
	uint32_t offset = (y * width + x) << 1;

	while (h--) {
		for (uint16_t i = w; i; i--) {
			framebuffer[offset++] = hi;
			framebuffer[offset++] = lo;
		}
		offset += (width - w) << 1;
	}
}

void ili9340_draw_line_v(uint16_t x, uint16_t y, uint16_t h, uint16_t color) 
{
	ili9340_fill_rect(x, y, 1, h, color);
}


void ili9340_draw_line_h(uint16_t x, uint16_t y, uint16_t w, uint16_t color) 
{
	ili9340_fill_rect(x, y, w, 1, color);
}

void ili9340_mkdirty(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	if (x0 < dirty_x0) dirty_x0 = x0;
	if (y0 < dirty_y0) dirty_y0 = y0;
	if (x1 > dirty_x1) dirty_x1 = x1;
	if (y1 > dirty_y1) dirty_y1 = y1;
}

void ili9340_update_display()
{
	if (dirty_x0 >= width || dirty_x1 >= width ||
			dirty_y0 >= height || dirty_y1 >= height) {
		dirty_x0 = 0;
		dirty_x1 = width - 1;
		dirty_y0 = 0;
		dirty_y1 = height - 1;
	}
	if (dirty_x0 > dirty_x1) {
		dirty_x0 = 0;
		dirty_x1 = width - 1;
	}
	if (dirty_y0 > dirty_y1) {
		dirty_y0 = 0;
		dirty_y1 = height - 1;
	}

	ili9340_set_addr_window(dirty_x0, dirty_y0, dirty_x1, dirty_y1);

	uint32_t offset = 2 * (dirty_y0 * width + dirty_x0);
	uint32_t len = 2 * (dirty_x1 - dirty_x0 + 1);

	while (dirty_y0 <= dirty_y1) {
		bcm2835_spi_writenb(framebuffer + offset, len);
		offset += width << 1;
		dirty_y0++;
	}

	dirty_x0 = width;
	dirty_x1 = 0;
	dirty_y0 = height;
	dirty_y1 = 0;
}

uint16_t ili9340_get_width()
{
	return width;
}

uint16_t ili9340_get_height()
{
	return height;
}

void ili9340_set_rotation(uint8_t m) 
{
	rotation = m % 4; // can't be higher than 3
	switch (rotation) {
		case 0:
			ili9340_write_command(ILI9340_MADCTL, 1, ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
			width  = ILI9340_TFTWIDTH;
			height = ILI9340_TFTHEIGHT;
			break;
		case 1:
			ili9340_write_command(ILI9340_MADCTL, 1, ILI9340_MADCTL_MV | ILI9340_MADCTL_BGR);
			width  = ILI9340_TFTHEIGHT;
			height = ILI9340_TFTWIDTH;
			break;
		case 2:
			ili9340_write_command(ILI9340_MADCTL, 1, ILI9340_MADCTL_MY | ILI9340_MADCTL_BGR);
			width  = ILI9340_TFTWIDTH;
			height = ILI9340_TFTHEIGHT;
			break;
		case 3:
			ili9340_write_command(ILI9340_MADCTL, 1, ILI9340_MADCTL_MV | ILI9340_MADCTL_MY 
					| ILI9340_MADCTL_MX | ILI9340_MADCTL_BGR);
			width  = ILI9340_TFTHEIGHT;
			height = ILI9340_TFTWIDTH;
			break;
	}

	dirty_x0 = width;
	dirty_x1 = 0;
	dirty_y0 = height;
	dirty_y1 = 0;
}

void ili9340_init(void)
{
	bcm2835_spi_begin();
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                  
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64); 
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_gpio_fsel(25, BCM2835_GPIO_FSEL_OUTP); 

	ili9340_write_command(0xEF, 3, 0x03, 0x80, 0x02);
	ili9340_write_command(0xCF, 3, 0x00 , 0XC1 , 0X30);
	ili9340_write_command(0xED, 4, 0x64 , 0x03 , 0X12 , 0X81);
	ili9340_write_command(0xE8, 3, 0x85 , 0x00 , 0x78);
	ili9340_write_command(0xCB, 5, 0x39 , 0x2C , 0x00 , 0x34 , 0x02);
	ili9340_write_command(0xF7, 1, 0x20);
	ili9340_write_command(0xEA, 2, 0x00 , 0x00);

	/* Power Control 1 */
	ili9340_write_command(0xC0, 1, 0x23);

	/* Power Control 2 */
	ili9340_write_command(0xC1, 1, 0x10);

	/* VCOM Control 1 */
	ili9340_write_command(0xC5, 2, 0x3e, 0x28);

	/* VCOM Control 2 */
	ili9340_write_command(0xC7, 1, 0x86);

	/* COLMOD: Pixel Format Set */
	/* 16 bits/pixel */
	ili9340_write_command(0x3A, 1, 0x55);

	/* Frame Rate Control */
	/* Division ratio = fosc, Frame Rate = 79Hz */
	ili9340_write_command(0xB1, 2, 0x00, 0x18);

	/* Display Function Control */
	ili9340_write_command(0xB6, 3, 0x08, 0x82, 0x27);

	/* Gamma Function Disable */
	ili9340_write_command(0xF2, 1, 0x00);

	/* Gamma curve selected  */
	ili9340_write_command(0x26, 1, 0x01);

	/* Positive Gamma Correction */
	ili9340_write_command(0xE0, 15,
			0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
			0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00);

	/* Negative Gamma Correction */
	ili9340_write_command(0xE1, 15,
			0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
			0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F);

	/* Sleep OUT */
	ili9340_write_command(0x11, 0);

	bcm2835_delay(120);

	/* Display ON */
	ili9340_write_command(0x29, 0);

	ili9340_set_rotation(0);

	dirty_x0 = width;
	dirty_x1 = 0;
	dirty_y0 = height;
	dirty_y1 = 0;
}

void ili9340_close(void)
{
	bcm2835_spi_end();
}

