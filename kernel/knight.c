#include "./sub_boss/knight.h"
#include "./sub_boss/knight_actions.h"
#include "../uart/uart1.h"
int i = 0;
int j = 0;
human control_knight(human player1, human knight, int *move_signal, int player1_prior_x, int player1_prior_y){
    //uart_dec(player1_prior_x- knight.x);
    //uart_sendc('\n');
    
    if(*move_signal == 1){
        drawRectARGB32(knight.x, knight.y-10, knight.x + 42, knight.y + 50, 0x00000000, 1);
        
        if(player1_prior_x < knight.x){
            knight.x -= 5;
            drawGameAsset(i,knight.x,knight.y,knight_right_resolution[0],knight_right_resolution[1],knight_right_allArray);
        }else if(player1_prior_x > knight.x){
            knight.x += 5;
            drawGameAsset(i,knight.x,knight.y,knight_right_resolution[0],knight_right_resolution[1],knight_left_allArray);
        }

        if(player1_prior_y < knight.y){
            knight.y -= 5;
        }else if(player1_prior_y > knight.y){
            knight.y += 5;
        }

        if((absolute(player1_prior_x - knight.x) < 40) && absolute(player1_prior_y- knight.y) < 50){
            *move_signal = 2;
        }

        i++;
        if(i > knight_right_allArray_LEN-1){
            i = 0;
        }
    }else if(*move_signal == 2){
        drawRectARGB32(knight.x-10, knight.y-10, knight.x + 50, knight.y + 50, 0x00000000, 1);
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
    }else{
        drawRectARGB32(knight.x, knight.y, knight.x + 75, knight.y + 75, 0x00000000, 1);
        drawGameAsset(0,knight.x,knight.y,knight_down_resolution[0],knight_down_resolution[1],knight_down_allArray);
    }
    return knight;
}