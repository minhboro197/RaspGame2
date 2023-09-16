#include "../uart/uart1.h"
#include "character.h"
#include "framebf.h"
#include "map.h"
#include "magewalking.h"
// the compiler randomly ask for memcpy so i included it here
void *memcpy(void *dest, const void *src, int count)
{
    char *d = dest;
    const char *s = src;
    while (count--)
    {
        *d++ = *s++;
    }
    return dest;
}

void *memset(void *dest, int value, int count)
{
    unsigned char *ptr = (unsigned char *)dest;
    unsigned char byteValue = (unsigned char)value;

    for (int i = 0; i < count; i++)
    {
        ptr[i] = byteValue;
    }

    return dest;
}

void npc1_plant_bomb(human npc1, human player)
{
    if (absolute(npc1.x - player.x) < 50 && absolute(npc1.y - player.y) < 50)
    {
        uart_puts("detected by enemy");
    }
}

void print_map(int map[][28])
{
    for (int i = 0; i < 18; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            uart_dec(map[i][j]);
            uart_sendc(',');
        }
        uart_sendc('\n');
    }
}

void character_take_damage(human *characters[], int *got_hit_player,int *take_damaged_once){
        for(int i = 0; i <3; i++){
            if(*take_damaged_once){
                if(*got_hit_player == i){
                    characters[i]->health -= 1;
                    characters[i]->got_hit = 1;
                    *got_hit_player = -1;
                    *take_damaged_once = 0;
                }
            }
        }
        
}

void play_game()
{
    moves npc1_moves[] = {
        {'d', 85},
        {'a', 100},
        {'s', 100},
        {'w', 100}};

    moves npc2_moves[] = {
        {'w', 100},
        {'d', 80},
        {'s', 100},
        {'a', 100}};

    moves* all_npc_moves[] = {npc1_moves, npc2_moves};

    // wall block width and height is 38 and 46
    // span npc1 in the below formar
    human player1 = character1_init(38, 46 * 3, 9,0,8,mage_width,mage_height);
    human npc1 = character1_init(38 * 10, 46 * 10, 9,1,8,mage_width,mage_height);
    human npc2 = character1_init(38 * 5, 46 * 9, 9,1,8,mage_width,mage_height);

    // 1 seconds = 1000000
    set_wait_timer(1, 10000); // set 10ms


    int map2[][28] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 0, 0, 2, 0, 0, 4, 0, 0, 1, 0, 0, 0, 0, 4, 0, 2, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 4, 0, 2, 2, 2, 2, 2, 0, 4, 0, 2, 0, 1, 0, 1},
        {1, 0, 1, 3, 3, 3, 3, 3, 3, 3, 0, 0, 4, 0, 0, 0, 0, 0, 2, 0, 4, 0, 2, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 2, 0, 4, 0, 2, 0, 1, 0, 1},
        {1, 0, 1, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2, 0, 4, 0, 2, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 3, 0, 2, 0, 4, 0, 2, 0, 1, 0, 1},
        {1, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3, 0, 2, 0, 4, 0, 2, 0, 1, 0, 1},
        {1, 0, 4, 4, 4, 4, 4, 4, 4, 4, 0, 1, 0, 2, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1, 0, 2, 0, 0, 3, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1},
        {1, 0, 4, 4, 4, 4, 4, 4, 4, 4, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 4, 4, 4, 4, 4, 4, 4, 0, 3, 0, 2, 2, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}};

    int map3[][28] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {3, 0, 0, 0, 0, 0, 4, 4, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 3},
        {3, 0, 2, 2, 2, 0, 4, 0, 0, 3, 0, 2, 2, 2, 0, 4, 2, 2, 2, 4, 0, 2, 2, 2, 0, 3, 3},
        {3, 0, 0, 0, 2, 0, 4, 0, 2, 4, 0, 2, 0, 0, 0, 4, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 3},
        {4, 4, 4, 0, 2, 0, 4, 0, 2, 0, 0, 0, 0, 2, 4, 4, 2, 4, 0, 2, 4, 2, 2, 0, 3, 0, 4},
        {3, 0, 0, 0, 2, 0, 3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 3},
        {3, 0, 3, 0, 2, 3, 3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3},
        {3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3},
        {3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3},
        {2, 2, 2, 2, 2, 0, 4, 4, 4, 0, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0, 2, 2, 2, 2, 2, 2},
        {3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3},
        {3, 0, 3, 0, 4, 4, 0, 0, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 0, 4, 4, 4, 4, 4, 4, 0, 3},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 3},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}};

    int once = 0;
    int take_damaged_once = 1;
    int got_hit_player = -1;

    while (1)
    {   
        
        // Draw map
        if (once == 0)
        {
            once = 1;
            draw_map_from_array(map2);
        }

        human *characters[] = {&player1,&npc1,&npc2};   // Write only
        human characters2[] = {*characters[0],*characters[1],*characters[2]};  // Read only
        
        char c = getUart();

        *characters[0] = controlCharater(characters2, *characters[0], c, tracking_player_on_map(*characters[0], map2, c),&got_hit_player, mage_walking_allArray);
        
        character_take_damage(&characters,&got_hit_player,&take_damaged_once);

        int timer = set_wait_timer(0, 0);
        if (timer)
        {
            if ((timer % 10) == 0)
            { // every 100ms
                //*characters[0] = controlCharater(characters2, *characters[0],'t', 0, &got_hit_player,mage_walking_allArray);
                
                for(int i = 0; i< 3; i++){
                    if(characters[i]->got_hit){
                        *characters[i] = controlCharater(characters2, *characters[i],'t', 0, &got_hit_player,mage_walking_allArray);
                        characters[i]->got_hit = 0;
                        //uart_dec(absolute(characters[i]->health));
                        uart_dec(i);
                        uart_sendc('\n');
                    }else{
                        if(i>0){    // exclude player
                            *characters[i] = move(characters2, *characters[i], all_npc_moves[i-1], sizeof(npc1_moves) / sizeof(npc1_moves[0]), 0,&got_hit_player,mage_walking_allArray); 
                        }
                    }

                }
                
                
                //npc2 = move(characters2, npc2, npc2_moves, sizeof(npc2_moves) / sizeof(npc2_moves[0]), 0,&got_hit_player,mage_walking_allArray);
                
                //int x = npc_hit_detection(characters2, npc1.x,npc1.y);


                for (int i = 0; i < player1.bomb_num; i++)
                {
                    player1.bomb[i].delay++;
                    if (player1.bomb[i].delay > 20)
                    { // delay 2 seconds
                        player1.bomb[i].frame++;
                        if (player1.bomb[i].frame > 11)
                        {
                            player1.bomb[i].frame = 0;
                            player1.bomb[i].state = 0;
                            player1.bomb[i].delay = 0;
                            take_damaged_once = 1;
                        }
                    }
                }
            }

            set_wait_timer(1, 10000); // reset 10ms timer
        }
    }
}

void main()
{
    // set up serial console
    uart_init();
    // say hello
    uart_puts("\n\nHello World!\n");
    // Initialize frame buffer
    framebf_init();
    draw_stats();
    // echo everything back
    play_game();
}