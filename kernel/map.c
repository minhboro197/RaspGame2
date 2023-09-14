#include "character.h"
#include "map.h"
#include "wall_block.h"
#include "../uart/uart1.h"

void draw_map(int map[][28]){
    for(int i = 0; i < 18; i++){
        map[i][0]= 1;
        map[i][23] = 1;

        drawGameAsset(0, 0 + SCREEN_WIDTH_OFFSET,i*48 ,38,46,wall_block_allArray);
        drawGameAsset(0, 986 - SCREEN_WIDTH_OFFSET,i*48,38,46,wall_block_allArray);
        //drawRectARGB32(0,i*40,40,40,0x00AA0000,1);
        //drawRectARGB32(986,i*40,40,40,0x00AA0000,1);
    }
    for(int i = 0; i < 25; i++){
        map[2][i] = 1;
        map[15][i] = 1;
        drawGameAsset(0, i*40 + SCREEN_WIDTH_OFFSET,0 + SCREEN_HEIGHT_OFFSET,38,46,wall_block_allArray);
        drawGameAsset(0, i*40 + SCREEN_WIDTH_OFFSET,720,38,46,wall_block_allArray);
    }

}

void draw_map_from_array(int map[][28]){
        for(int i = 0; i < 18; i++){
            for(int j = 0; j < 28; j++){
                if(map[i][j] > 0){
                    drawGameAsset(map[i][j] - 1, j*block_width,i*block_height,block_width,block_height,wall_block_allArray);
                }
            }
        }
}
