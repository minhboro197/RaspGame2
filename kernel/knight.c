#include "./sub_boss/knight.h"
#include "./sub_boss/knight_actions.h"
#include "boss/dragons.h"
#include "../uart/uart1.h"
#include "dragon_fireball.h"

int i = 0;
int j = 0;
int k = 0;
int l = 0;

human control_dragon(human *player1, human dragon,Explosion *dragon_explosion, int player1_prior_x, int player1_prior_y,int speed){
    if(dragon.is_alive == 0){
        if(dragon.x == dragon.prior_x){
            drawRectARGB32(dragon.prior_x-30,dragon.prior_y-30,dragon.prior_x+dragon.frame_width+30,dragon.prior_y + dragon.frame_height+30,0x00000000,1);
        
        }
        dragon.x = 500;
        dragon.y = 0;
        return dragon;
    }


    drawRectARGB32(dragon_explosion->prior_x, dragon_explosion->prior_y, dragon_explosion->prior_x + dragon_explosion_width, dragon_explosion->prior_y + dragon_explosion_height, 0x00000000, 1);

    if(absolute(dragon_explosion->x - player1->x) < 25 && absolute(dragon_explosion->y - player1->y )< 25 ){
        player1->health--;
        dragon_explosion->x = dragon.x +dragon.frame_width/2;
        dragon_explosion->y = dragon.y +dragon.frame_height/2;
    }

    if(player1->x/38 > 12){
        drawGameAsset(dragon.frame,400,200,dragon_boss_width,dragon_boss_height,dragon_boss_right_allArray);
    }else{
        drawGameAsset(dragon.frame,400,200,dragon_boss_width,dragon_boss_height,dargon_boss_allArray);
    }

     dragon.frame++;
    if(dragon.frame > 7){
        dragon.frame = 0;
    }

     if(player1_prior_x < dragon_explosion->x){
            dragon_explosion->x -= speed;
    }else if(player1_prior_x > dragon_explosion->x){
        dragon_explosion->x += speed;
    }

    if(player1_prior_y < dragon_explosion->y){
        dragon_explosion->y -= speed;
    }else if(player1_prior_y > dragon_explosion->y){
        dragon_explosion->y += speed;
    }
    drawGameAsset(dragon_explosion->frame,dragon_explosion->x,dragon_explosion->y,dragon_explosion_width,dragon_explosion_height,dragon_explosion_allArray);
    dragon_explosion->frame++;
    if(dragon_explosion->frame > 7){
        dragon_explosion->frame = 0;
    }
    
    dragon_explosion->prior_x = dragon_explosion->x;
    dragon_explosion->prior_y = dragon_explosion->y;

    if(absolute(dragon_explosion->x - player1_prior_x) < 20 && absolute(dragon_explosion->y - player1_prior_y )< 20 ){
        dragon_explosion->x = dragon.x + dragon.frame_width/2;
        dragon_explosion->y = dragon.y + dragon.frame_height/2;
    }

    return dragon;
}
human control_knight(human player1, human knight, int *move_signal, int player1_prior_x, int player1_prior_y, int speed){

    if(knight.is_alive == 0){
        knight.x = 500;
        knight.y = 0;
        return knight;
    }
    
    if(*move_signal == 1){
        drawRectARGB32(knight.prior_x, knight.prior_y, knight.prior_x + 55, knight.prior_y + 40, 0x00000000, 1);
    }else{
        drawRectARGB32(knight.prior_x, knight.prior_y, knight.prior_x + 80, knight.prior_y + 80, 0x00000000, 1);
    }

    if(*move_signal == 1){
        knight.prior_x = knight.x;
        knight.prior_y = knight.y;
        
        if(player1_prior_x < knight.x){
            knight.x -= speed;
            drawGameAsset(i,knight.x,knight.y,knight_right_resolution[0],knight_right_resolution[1],knight_right_allArray);
        }else if(player1_prior_x > knight.x){
            knight.x += speed;
            drawGameAsset(i,knight.x,knight.y,knight_right_resolution[0],knight_right_resolution[1],knight_left_allArray);
        }

        if(player1_prior_y < knight.y){
            knight.y -= speed;
        }else if(player1_prior_y > knight.y){
            knight.y += speed;
        }

        if((absolute(player1_prior_x - knight.x) < 30) && absolute(player1_prior_y- knight.y) < 30){
            *move_signal = 2;
        }

        i++;
        if(i > knight_right_allArray_LEN-1){
            i = 0;
        }
    }else if(*move_signal == 2){
        
        if(absolute(player1_prior_x - knight.x) > absolute(player1_prior_y - knight.y)){
            if(player1_prior_x < knight.x){
                
                drawGameAsset(j,knight.x,knight.y,knight_slash_right_resolution[0],knight_slash_right_resolution[1],knight_slash_right_allArray);
            }else if(player1_prior_x > knight.x){
   
                drawGameAsset(j,knight.x,knight.y,knight_slash_left_resolution[0],knight_slash_left_resolution[1],knight_slash_left_allArray);
            }
        }else{
            if(player1_prior_y < knight.y){
         
                drawGameAsset(j,knight.x,knight.y,knight_slash_up_resolution[0],knight_slash_up_resolution[1],knight_slash_up_allArray);
            }else if(player1_prior_y > knight.y){
        
                drawGameAsset(j,knight.x,knight.y,knight_slash_down_resolution[0],knight_slash_down_resolution[1],knight_slash_down_allArray);
            }
        }

        j++;
        if(j > knight_slash_down_allArray_LEN-1){
            j = 0;
        }
    }else if(*move_signal == 3){
        k++;
        drawGameAsset(k,knight.x,knight.y,knight_die_resolution[0],knight_die_resolution[1],knigh_die_allArray);
        if(k > knigh_die_allArray_LEN-2){
            drawRectARGB32(knight.x+5, knight.y, knight.x + 75, knight.y + 75, 0x00000000, 1);
            k = 0;
            knight.is_alive = 0;
        }
    }else if(*move_signal == 4){
        l++;
        drawGameAsset(l,knight.x,knight.y,knight_hurt_resolution[0],knight_hurt_resolution[1],knight_hurt_allArray);
        if(l > knight_hurt_allArray_LEN-1){
            l = 0;
        }
    }
    else{
        drawGameAsset(0,knight.x,knight.y,knight_down_resolution[0],knight_down_resolution[1],knight_down_allArray);
    }
    return knight;
}