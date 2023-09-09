
#include "character.h"
#include "bomb.h"
#include "magewalking.h"
#include "framebf.h"
#include "bomb_explosion.h"

int collision_detection(human humans[], unsigned int object_x, unsigned int object_y){
   for(int i = 0; i < 3; i++){
        if(absolute(humans[i].x - object_x) < 40 && absolute(humans[i].y - object_y) < 40){

            return 1;
        }
    }
    return 0;
}

void drawGameAsset(int frame, unsigned int offset_x,unsigned int offset_y, unsigned int width, unsigned int height, const unsigned long* frame_array[]){
    
    int index =0;
    for(int i = 0 + offset_x; i < width + offset_x; i++){
        for(int j = 0 + offset_y; j < height + offset_y; j++){
            drawPixelARGB32(i,j,frame_array[frame][index]);
            index++;
        }
    }
}

human plant_bomb(human characters[],human player1, char c){
    if(c == 'j'){
            if(player1.bomb_num > 4){
                player1.bomb_num = 4;
            }else{
                player1.bomb[player1.bomb_num].x = player1.x;
                player1.bomb[player1.bomb_num].y = player1.y;
                player1.bomb[player1.bomb_num].state = 1;
            }
            player1.bomb_num++;
        }
    for(int i = 0; i < player1.bomb_num; i++){
        int collision_bomb[4] = {0,0,0,0};
        if(player1.bomb[i].state == 1){
            drawGameAsset(player1.bomb[i].frame, player1.bomb[i].x,player1.bomb[i].y,46,49,bomb_allArray);
            if(player1.bomb[i].frame > 4){
                for(int j =0; j < 4; j++){  // 4 here is max range change this later
                    int bomb_directions[4][2] = {{player1.bomb[i].x, player1.bomb[i].y - 48*j},
                                                {player1.bomb[i].x, player1.bomb[i].y + 48*j},
                                                {player1.bomb[i].x - 48*j,player1.bomb[i].y},
                                                {player1.bomb[i].x + 48*j,player1.bomb[i].y}};

                    for(int k = 0; k < 4; k++){
                        if(collision_detection(characters, bomb_directions[k][0], bomb_directions[k][1]) == 0 && collision_bomb[k] == 0){
                            
                            drawGameAsset(player1.bomb[i].frame-4, bomb_directions[k][0],bomb_directions[k][1],48,48,bomb_explosion_allArray);
                        }else{
                            collision_bomb[k] = k;
                            drawGameAsset(7, bomb_directions[k][0],bomb_directions[k][1],48,48,bomb_explosion_allArray);  // blank if hit
                        }
                    }
                }
            }
        }
        
    }
    return player1;
}

human character1_init(int x, int y){
    human character1;
    character1.prior_x = x;
    character1.prior_y = y;
    character1.x = x;
    character1.y = y;
    character1.moveup_frame_offset = 0;
    character1.movedown_frame_offset = 18;
    character1.moveleft_frame_offset = 27;
    character1.moveright_frame_offset = 9;
    character1.offset = 8;
    character1.move_index = 0;
    character1.bomb_num = 0;
    return character1;
}

int frame = 0;
int offset_move = 0;

unsigned int absolute(int num){
    if(num < 0){
        num = num * -1;
        return num;
    }
    return num;
}
int bomb_planted = 0;

human controlCharater(human characters[],human player1, char c){
    if(c == 'd'){
        
        player1.offset = player1.moveright_frame_offset + frame;
        frame ++;
        if(frame > 8){
            frame = 0;
        }
        drawRectARGB32(player1.x,player1.y,player1.x+35,player1.y + 60,0x00000000,1);
        player1.x += 5;
    }else if(c == 'a'){
        player1.offset = player1.moveleft_frame_offset + frame;
        frame++;
        if(frame > 8){
            frame = 0;
        }
        drawRectARGB32(player1.x,player1.y,player1.x+35,player1.y + 60,0x00000000,1);
        player1.x -= 5;
    }else if(c == 'w'){
        player1.offset = player1.moveup_frame_offset + frame;
        frame++;
        if(frame > 8){
            frame = 0;
        }
        drawRectARGB32(player1.x,player1.y,player1.x+35,player1.y + 60,0x00000000,1);
        player1.y -= 5;
    }else if(c == 's'){
        player1.offset = player1.movedown_frame_offset + frame;
        frame++;
        if(frame > 8){
            frame = 0;
        }
        drawRectARGB32(player1.x,player1.y,player1.x+35,player1.y + 60,0x00000000,1);
        player1.y += 5;
    }else if(c == 'j' || player1.bomb_num){
        player1 = plant_bomb(characters,player1,c);
    }
    
    drawGameAsset(player1.offset, player1.x, player1.y, 35,58, mage_walking_allArray);
    
    
    return player1;
}

human move(human players[],human npc,moves moves[], unsigned int move_size){
    
    human temp = controlCharater(players,npc, moves[npc.move_index].direction);
    
    if(absolute(temp.x - temp.prior_x) == moves[temp.move_index].distance){
        temp.prior_x = temp.x;
        temp.move_index = temp.move_index +1;
        
    }else if(absolute(temp.y - temp.prior_y) == moves[temp.move_index].distance){
        temp.prior_y = temp.y;
        temp.move_index = temp.move_index +1;
    }
    
    if(temp.move_index > move_size-1){
        temp.move_index = 0;
    }
    return temp;
}