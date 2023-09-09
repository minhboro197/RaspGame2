#include "character.h"
#include "map.h"
#include "wall_block.h"
#define SCREEN_WIDTH_OFFSET 12

void draw_map(){
    int j =1;
    for(int i = 0; i < 23; i++){
        if(i < 16){
            drawGameAsset(0, 0 + SCREEN_WIDTH_OFFSET,i*48,38,46,wall_block_allArray);
            drawGameAsset(0, 986 - SCREEN_WIDTH_OFFSET,i*48,38,46,wall_block_allArray);
            //drawRectARGB32(0,i*40,40,40,0x00AA0000,1);
            //drawRectARGB32(986,i*40,40,40,0x00AA0000,1);
        }
        drawGameAsset(0, j*40 + SCREEN_WIDTH_OFFSET,0,38,46,wall_block_allArray);
        drawGameAsset(0, j*40 + SCREEN_WIDTH_OFFSET,720,38,46,wall_block_allArray);
        j++;
    }
    
}   