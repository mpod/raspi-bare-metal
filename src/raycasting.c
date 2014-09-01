#include <math.h>
#include "raycasting.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define ABS(A) ((A) < 0 ? (-A) : (A))
#define PI 3.141592653589793238463
#define TWOPI (2.0 * PI)

const int map[24][32] = {
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,3,0,3,0,0,1,1,1,2,1,1,1,1,1,2,1,1,1,2,1,0,0,0,0,0,0,0,0,1},
	{1,0,0,3,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,1,1,1,1},
	{1,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,3,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,3,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,3,3,3,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{1,0,0,0,0,0,0,0,0,3,3,3,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,3,3,3,0,0,3,3,3,0,0,0,0,0,0,0,0,0,3,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,3,3,3,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,4,0,0,4,2,0,2,2,2,2,2,2,2,2,0,2,4,4,0,0,4,0,0,0,0,0,0,0,1},
	{1,0,0,4,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,4,0,0,0,0,0,0,0,1},
	{1,0,0,4,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,4,0,0,0,0,0,0,0,1},
	{1,0,0,4,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,4,0,0,0,0,0,0,0,1},
	{1,0,0,4,3,3,4,2,2,2,2,2,2,2,2,2,2,2,2,2,4,3,3,4,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int map_width;
int map_height;
int screen_width;
int screen_height;
int cell_size;
float proj_plane_dist;

struct player player;

void init_world(int scr_width, int scr_height)
{
	player = (struct player) {
		.x = 96., 
		.y = 130., 
		.rot = 30. * PI / 180., 
		.rot_dir = 0, 
		.move_dir = 0, 
		.move_speed = 18., 
		.rot_speed = 6. * PI / 180
	};
	map_width = 32;
	map_height = 24;
	screen_height = scr_height;
	screen_width = scr_width;
	cell_size = 64;
	proj_plane_dist = 277.; 
}

float cast_single_ray(double ray_angle) 
{
	// first make sure the angle is between 0 and 360 degrees
	ray_angle = fmod(ray_angle, TWOPI);
	if (ray_angle < 0) ray_angle += TWOPI;
	int right = (ray_angle > TWOPI * 0.75) || (ray_angle < TWOPI * 0.25);
	int up = ray_angle < PI;
	double tan_angle = tan(ray_angle);

	float dx, dy;
	float x, y;
	int map_x;	
	int map_y;
	float dist = 0;

	// Vertical lines
	if (right)
		x = floor(player.x / cell_size) * cell_size + cell_size;
	else
		x = floor(player.x / cell_size) * cell_size - 1;
	y = player.y + (player.x - x) * tan_angle;			
	map_x = x / cell_size;
	map_y = y / cell_size;
	dx = right ? cell_size : -cell_size; 
	dy = up ? -cell_size * ABS(tan_angle) : cell_size * ABS(tan_angle); 	
	while (x >= 0 && x < map_width * cell_size && y >= 0 && y < map_height * cell_size) {
		if (map[map_y][map_x] > 0) {
			break;
		}
		x += dx;
		y += dy;
		map_x = x / cell_size;
		map_y = y / cell_size;
	}
	dist = sqrt((player.x - x) * (player.x - x) + (player.y - y) * (player.y - y));

	// Horizontal lines
	if (up)
		y = floor(player.y / cell_size) * cell_size - 1;
	else
		y = floor(player.y / cell_size) * cell_size + cell_size;
	x = player.x + (player.y - y) / tan_angle;
	map_x = x / cell_size;
	map_y = y / cell_size;
	dy = up ? -cell_size : cell_size;
	dx = right ? cell_size / ABS(tan_angle) : -cell_size / ABS(tan_angle);
	while (x >= 0 && x < map_width * cell_size && y >= 0 && y < map_height * cell_size) {
		if (map[map_y][map_x] > 0) {
			break;
		}
		x += dx;
		y += dy;
		map_x = x / cell_size;
		map_y = y / cell_size;
	}

	dist = MIN(dist, sqrt((player.x - x) * (player.x - x) + (player.y - y) * (player.y - y)));
	dist = dist * cos(player.rot - ray_angle);

	return (float)cell_size / dist * proj_plane_dist;
}

void cast_rays(draw_vert_line_cb draw_vert_line, update_display_cb update_display) 
{
	float proj_slice_height;
	float dangle = 60. * PI / 180. / screen_width;

	for (int x = 0; x < screen_width; x++) {
		float ray_angle = player.rot + 30. * PI / 180. - dangle * x;
		proj_slice_height = cast_single_ray(ray_angle);

		int y1 = screen_height / 2 - proj_slice_height / 2;
		int y2 = screen_height / 2 + proj_slice_height / 2;

		y1 = MIN(y1, screen_height);
		y2 = MIN(y2, screen_height);

		if (y1 >= screen_height) y1 = screen_height - 1;
		if (y2 >= screen_height) y2 = screen_height - 1;
		if (y1 < 0) y1 = 0;
		if (y2 < 0) y2 = 0;

		draw_vert_line(x, 0, y1, DARK_COLOR);
		draw_vert_line(x, y1, y2 - y1, LIGHT_COLOR); 
		draw_vert_line(x, y2, screen_height - y2, DARK_COLOR);
	}
	update_display();
}

int is_blocking(float x, float y) 
{
	if (y < 0 || y >= map_height * cell_size || x < 0 || x >= map_width * cell_size)
		return 1;
	return (map[(int)floor(y / cell_size)][(int)floor(x / cell_size)] != 0); 
}

void move_player() 
{
	float move_step = player.move_dir * player.move_speed;

	player.rot += player.rot_dir * player.rot_speed;

	while (player.rot < 0) player.rot += TWOPI;
	while (player.rot >= TWOPI) player.rot -= TWOPI;

	float newx = player.x + cos(player.rot) * move_step;
	float newy = player.y - sin(player.rot) * move_step;

	if (is_blocking(newx, newy)) {
		return;
	}

	player.x = newx; 
	player.y = newy;
	player.move_dir = 0;
	player.rot_dir = 0;
}


