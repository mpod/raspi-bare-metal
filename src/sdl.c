#include <SDL2/SDL.h>
#include "raycasting.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Renderer* renderer;

void draw_vert_line(int x, int y, int h, int color_mode)
{
	if (color_mode) {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawLine(renderer, x, y, x, y + h);
	}
}

void update_display(void)
{
	SDL_RenderPresent(renderer);
	SDL_Delay(1000 / 30);
}

int run(void)
{
	int quit = 0;
	init_world(SCREEN_WIDTH, SCREEN_HEIGHT);

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		printf("SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("SDL Raspberry Pi raycasting", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL){
		printf("CreateWindow: %s\n", SDL_GetError());
		return 2;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL){
		printf("CreateRenderer: %s\n", SDL_GetError());
		return 3;
	}

	SDL_Event e;
	while (!quit){
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				quit = 1;
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_a:
						player.rot_dir = 1.;
						break;
					case SDLK_d:
						player.rot_dir = -1.;
						break;
					case SDLK_w:
						player.move_dir = 1.;
						break;
					case SDLK_s:
						player.move_dir = -1.; 
						break;
					case SDLK_ESCAPE:
						quit = 1;
						break;
					default:
						break;
				}
			} else if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
					case SDLK_a:
					case SDLK_d:
						player.rot_dir = 0;
						break;
					case SDLK_w:
					case SDLK_s:
						player.move_dir = 0;
						break;
					default:
						break;
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		move_player();
		cast_rays(draw_vert_line, update_display);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
