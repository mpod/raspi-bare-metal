#define DARK_COLOR 0
#define LIGHT_COLOR 1

struct player { 
	float x;
	float y;
	float rot;
	float move_dir;
	float rot_dir;
	float move_speed;
	float rot_speed;
};

extern struct player player;

typedef void (*draw_vert_line_cb)(int x, int y, int h, int color_mode);

typedef void (*update_display_cb)();

extern void init_world(int scr_height, int scr_width);
extern void cast_rays(draw_vert_line_cb draw_vert_line, update_display_cb update_display); 
extern void move_player(); 
