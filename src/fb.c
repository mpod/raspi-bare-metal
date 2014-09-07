#include "bcm2835.h"
#include "raycasting.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

uint16_t* framebuffer;

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
			framebuffer[i * SCREEN_WIDTH + x] = 0x7E0;
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
	bcm2835_init();
	bcm2835_aux_muart_init();

	// init LED 
	bcm2835_gpio_fsel(16, BCM2835_GPIO_FSEL_OUTP); 

	framebuffer = bcm2835_fb_init(SCREEN_WIDTH, SCREEN_HEIGHT);

	init_world(SCREEN_WIDTH, SCREEN_HEIGHT);
	cast_rays(draw_vert_line, update_display); 

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
		//LED blinking
		//bcm2835_gpio_write(16, HIGH);
		//bcm2835_delay(1000);
		//bcm2835_gpio_write(16, LOW);
		//bcm2835_delay(1000);

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
		cast_rays(draw_vert_line, update_display); 
	}
	return 0;
}
