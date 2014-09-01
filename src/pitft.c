#include "bcm2835.h"
#include "ili9340.h"
#include "raycasting.h"

void draw_vert_line(int x, int y, int h, int color_mode)
{
	if (color_mode) {
		ili9340_draw_line_v(x, y, h, ILI9340_RED);
	} else { 
		ili9340_draw_line_v(x, y, h, ILI9340_BLACK); 
	}
}

int run(void) 
{
	bcm2835_init();
	bcm2835_aux_muart_init();

	ili9340_init();
	ili9340_set_rotation(3);

	uint16_t screen_height = ili9340_get_height();
	uint16_t screen_width = ili9340_get_width();

	init_world(screen_width, screen_height);
	cast_rays(draw_vert_line, ili9340_update_display); 

	bcm2835_gpio_fsel(22, BCM2835_GPIO_FSEL_INPT); 
	bcm2835_gpio_fsel(23, BCM2835_GPIO_FSEL_INPT); 
	bcm2835_gpio_fsel(21, BCM2835_GPIO_FSEL_INPT); 
	bcm2835_gpio_fsel(18, BCM2835_GPIO_FSEL_INPT); 
	bcm2835_gpio_set_pud(22, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_set_pud(23, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_set_pud(21, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_set_pud(18, BCM2835_GPIO_PUD_UP);

	int pin22_state = 1;
	int pin23_state = 1;
	int pin21_state = 1;
	int pin18_state = 1;

	while (1) {
		bcm2835_aux_muart_transfernb("in the loop");
		if (!bcm2835_gpio_lev(23) && pin23_state) {
			pin23_state = 0;
			player.rot_dir = 1.;
			bcm2835_aux_muart_transfernb("23 - left");
		} else if (bcm2835_gpio_lev(23) && !pin23_state) {
			pin23_state = 1;
		}

		if (!bcm2835_gpio_lev(22) && pin22_state) {
			pin22_state = 0;
			player.move_dir = 1.;
			bcm2835_aux_muart_transfernb("22 - up");
		} else if (!bcm2835_gpio_lev(22) && !pin22_state) {
			pin22_state = 1;
		}

		if (!bcm2835_gpio_lev(21) && pin21_state) {
			pin21_state = 0;
			player.move_dir = -1.;
			bcm2835_aux_muart_transfernb("21 - down");
		} else if (!bcm2835_gpio_lev(21) && !pin21_state) {
			pin21_state = 1;
		}

		if (!bcm2835_gpio_lev(18)) {
			pin18_state = 0;
			player.rot_dir = -1.;
			bcm2835_aux_muart_transfernb("18 - right");
		} else if (!bcm2835_gpio_lev(18) && !pin18_state) {
			pin18_state = 1;
		}

		move_player();
		cast_rays(draw_vert_line, ili9340_update_display); 
	}
	return 0;
}
