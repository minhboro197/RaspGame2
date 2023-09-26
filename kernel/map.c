#include "character.h"
#include "map.h"
#include "wall_block.h"
#include "../uart/uart1.h"
#include "stat.h"
#include "framebf.h"
#include "./player/profilepic.h"
#include "./boss/dragon_profile_pic.h"
void draw_map_from_array(int map[][28])
{
    for (int i = 0; i < 17; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            if (map[i][j] > 0)
            {
                drawGameAsset(map[i][j] - 1, j * block_width, i * block_height, block_width, block_height, wall_block_allArray);
            }
        }
    }
}

void draw_stats(int heart_count, int bomb_damge, int bomb_range) {
    drawRectARGB32(0,0,500,50,0x00000000,1);
    int offset = 100;
    int spacing = 5; // Spacing between each heart
    int heart_width = 30; // Width of the heart icon

    drawGameAsset(0, 0, 0, profile_pic_width, profile_pic_height, profile_pic_allArray);
    for (int i = 0; i < heart_count; i++) {
        int x_position = i * (heart_width + spacing);
        drawGameAsset(0,offset+ x_position, 0, heart_width, 30, heart_iconallArray);
    }
    char damge[3];
    char range[3];
    int_to_string(bomb_damge,damge,10);
    int_to_string(bomb_range,range,10);
    drawString(offset+0,50,"BOMB DAMAGE: ",0x0f);drawString(offset + 95,50,damge,0x0f);
    drawString(offset+0,70,"BOMB RANGE: ",0x0f);drawString(offset + 95,70,range,0x0f);
}

void draw_boss_stats(int heart_count){
    drawRectARGB32(1024 - dragon_profile_pic_width -500,0,1024 - dragon_profile_pic_width,50,0x00000000,1);
    int spacing = 5;
    drawGameAsset(0, 1024 - dragon_profile_pic_width, 0, dragon_profile_pic_width, dragon_profile_pic_height, dragon_profile_allArray);
    for (int i = 0; i < heart_count; i++) {
        int x_position = i * (blue_heart_width + spacing);
        drawGameAsset(0,(1024 - (dragon_profile_pic_width + blue_heart_width)) - x_position, 0, blue_heart_width, blue_heart_height, blue_heart_allArray);
    }
    drawString((1024 - (dragon_profile_pic_width + 100)),50,"BLUE DRAGON ",0x3);
}

void draw_game_status(int status){
    if(status == 1){
        drawGameAsset(0,150,350, victory_width, victory_height, victory_allArray);
    }else {
        drawGameAsset(0,350,350, you_die_width, you_die_height, you_die_allArray);
    }
}
void print_map(int map[][28]){
    for(int i = 0; i < 17; i++){
            for(int j = 0; j < 28; j++){
                uart_dec(map[i][j]);
                uart_puts(",  ");
                
            }
            uart_sendc('\n');
        }
}

