#define SCREEN_WIDTH_OFFSET 5
#define SCREEN_HEIGHT_OFFSET 48 * 2
#define block_width 38
#define block_height 46
void draw_map_from_array(int map[][28]);
void draw_stats(int heart_count, int bomb_damge, int bomb_range);
void print_map(int map[][28]);
void draw_boss_stats(int heart_count);
void draw_game_status(int status);