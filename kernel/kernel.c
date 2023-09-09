#include "../uart/uart1.h"
#include "character.h"
#include "framebf.h"
#include "map.h"
// the compiler randomly ask for memcpy so i included it here
void *memcpy(void *dest, const void *src, int count) {
    char *d = dest;
    const char *s = src;
    while (count--) {
        *d++ = *s++;
    }
    return dest;
}


void npc_plant_bomb(human npc, human player){
    if(absolute(npc.x - player.x) < 50 && absolute(npc.y - player.y) < 50){
        uart_puts("detected by enemy");
    }
}



void main(){
    moves npc1_moves[] ={
        {'d',200},
        {'s',200},
        {'a',200},
        {'w',200}
    };

    moves npc2_moves[] ={
        {'w',100},
        {'d',100},
        {'s',100},
        {'a',100}
    };

    human player1 = character1_init(50,50);
    human npc = character1_init(100,100);
    human npc2 = character1_init(500,500);
    
    // set up serial console
    uart_init();
    // say hello
    uart_puts("\n\nHello World!\n");
    // Initialize frame buffer
    framebf_init();

    // echo everything back
    // 1 seconds = 1000000
    set_wait_timer(1, 10000);   // set 10ms
    int bomb_delay = 0;
    int once = 0;
    while(1) {
        if(once == 0){
            once = 1;
            draw_map();
        }
        human characters[] = {player1,npc,npc2};
        char c =  getUart();
        player1 = controlCharater(characters,player1, c);
        int timer = set_wait_timer(0,0);
        //uart_dec(player1.bomb_num);
        //uart_sendc('\n');
        
        if(timer){
            if((timer %10) == 0){   // every 100ms
                npc = move(characters,npc, npc1_moves, sizeof(npc1_moves) / sizeof(npc1_moves[0]));
                npc = controlCharater(characters,npc, 't'); // refresh character
                npc2 = move(characters,npc2, npc2_moves, sizeof(npc2_moves) / sizeof(npc2_moves[0]));
                npc2 = controlCharater(characters,npc2, 't'); // refresh character

                npc_plant_bomb(npc,player1);
                npc_plant_bomb(npc2,player1);


                for(int i = 0; i < player1.bomb_num; i ++){
                    player1.bomb[i].delay++;
                    if(player1.bomb[i].delay > 20){ // delay 2 seconds
                        player1.bomb[i].frame++;
                        if(player1.bomb[i].frame > 11){
                            player1.bomb[i].frame = 0;
                            player1.bomb[i].state = 0;
                            player1.bomb[i].delay = 0;
                        }
                    }
                }
                    
            }
            
            set_wait_timer(1, 10000);   // reset 10ms timer
        }

    }
}