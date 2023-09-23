
#include "../uart/uart1.h"
#include "framebf.h"
#include "map.h"
#include "magewalking.h"
#include "boss/dragons.h"
#include "./sub_boss/knight.h"      // character.h here
#include "map_array.h"
#include "./npcs/goblin.h"
#include "./npcs/red_dude.h"
#include "./npcs/goblem.h"
#include "./npcs/death.h"

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


void reset_flood_map(int map[][28]){
        // Copy map
    for(int i = 0; i < 17; i++){
        for(int j = 0; j < 28; j++){
            if(map[i][j] > 0){
                flood_map[i][j] = -1;
            }
            else{
                flood_map[i][j] = -2;
            }
        }
    }

    // Initialize distances to infinity
    for (int i = 0; i < 17; i++) {
        for (int j = 0; j < 28; j++) {
            distances[i][j] = 2147483647;
        }
    }
}


int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

void dijkstra_find_route(int map[][28], int x, int y, int x_end, int y_end, int distance[][28]){
    if(map[x_end][y_end] != -2){
        return;
    }
    if(x == x_end && y == y_end){
        return;
    }

    for(int i =0; i < 4; i++){
        int next_move[] = {x + dx[i], y + dy[i]};

        if(next_move[0] < 17 && next_move[1] < 28 && map[next_move[0]][next_move[1]] != -1){
            int newDistant = distance[x][y] +1;

            if(newDistant < distance[next_move[0]][next_move[1]]){
                //uart_dec(next_move[0]);
                //uart_sendc(',');
                //uart_dec(next_move[1]);
                //uart_sendc('\n');
                distance[next_move[0]][next_move[1]] = newDistant;
                dijkstra_find_route(map,next_move[0], next_move[1],x_end,y_end, distance);
            }
        }
    }
}

void dijkstra_plan_route(int map[][28], int x, int y,int x_end, int y_end, int distance[][28], moves *npc_chase){
    if(map[x_end][y_end] != -2){
        return;
    }
    
    int current_x = x_end;
    int current_y = y_end;

    while(current_x != x || current_y != y){
        if((current_x == x -1 && current_y == y) || (current_x == x + 1 && current_y == y) || (current_x == x && current_y == y-1) ||(current_x == x && current_y == y+1) ){
            break;
        }
        
        for(int i = 0; i < 4; i++){
            int next_move[] = {current_x + dx[i], current_y + dy[i]};
            //uart_dec(next_move[0]);
            //uart_sendc(',');
            //uart_dec(next_move[1]);
            //uart_sendc('\n');
            if(next_move[0] < 17 && next_move[1] < 28 && distance[next_move[0]][next_move[1]] == distance[current_x][current_y] -1){
                
                current_x = next_move[0];
                current_y = next_move[1];
                map[current_x][current_y] = 8;
                //uart_puts("bro2\n");
                break;
            }
        }
    }

    int index = 0;
    
    current_x = x;
    current_y = y;
    while(1){
        int min_index = 0;
        //uart_dec(x);
        //uart_sendc(',');
        //uart_dec(y);
        //uart_sendc('\n');
        //uart_puts("broas\n");

            //uart_dec(current_x);
            //uart_sendc(',');
            //uart_dec(current_y);
            //uart_sendc('\n');
        if((current_x == x_end -1 && current_y == y_end) || (current_x == x_end + 1 && current_y == y_end) || (current_x == x_end && current_y == y_end-1) ||(current_x == x_end && current_y == y_end+1) ){
            break;
        }
        for(int i = 0; i< 4; i++){
            int next_move[] = {current_x + dx[i], current_y + dy[i]};
            if(map[next_move[0]][next_move[1]] == 8){
                map[next_move[0]][next_move[1]] = 0; // mark as travelled
                min_index = i;
            }

        }
        if(min_index == 0){
            current_x -= 1;
            npc_chase[index].direction = 'w';
        }else if(min_index == 1){
            current_x += 1;
            npc_chase[index].direction = 's';
        }else if(min_index == 2){
            current_y -= 1;
            npc_chase[index].direction = 'a';
        }else if(min_index == 3){
            current_y += 1;
            npc_chase[index].direction = 'd';
        }
        npc_chase[index].distance[0] = current_y;
        npc_chase[index].distance[1] = current_x;
        
        index++;
    }
}




int once = 1;
int take_damaged_once = 0;
int got_hit_player = -1;
void play_game(int map[][28])
{  
    draw_map_from_array(map);
    draw_stats(5);


    moves death_npc_chase[100];
    moves red_dude_npc_chase[100];
    moves goblem_npc_chase[100];

    moves *npc_chases[] = {&death_npc_chase, &goblem_npc_chase, &red_dude_npc_chase};

    moves npc1_moves[] = {
        {'d', {5,11}},
        {'a', {1,11}}};

    moves npc2_moves[] = {
        {'w', 100},
        {'d', 80},
        {'s', 100},
        {'a', 100}};
    
    moves* all_npc_moves[] = {npc1_moves, npc2_moves};
    

    // wall block width and height is 38 and 46
    // span characters in the below format
    human player1 = character1_init(block_width*1, block_height * 3, 9,0,8,mage_width,mage_height);

    human npc1 = character1_init(block_width * 1, block_height * 11, 3,0,2,death_width,death_height);
    human npc2 = character1_init(38 * 5, 46 * 9, 3,0,2,goblem_width,goblem_height);
    human npc3 = character1_init(38 * 15, 46 * 3, 3,0,2,red_dude_width,red_dude_height);

    
    //print_map(flood_map);

    int prior_player_health = player1.health;
    int game_status = 0;

    int player_prior_x[] = {0,0,0};
    int player_prior_y[] = {0, 0,0};
    int npc_timer[] = {150,200,250};

    // 1 seconds = 1000000
    set_wait_timer(1, 10000); // set 10ms
    while (1)
    {   
        if(prior_player_health != player1.health){
            drawRectARGB32(0,0,500,50,0x00000000,1);
            draw_stats(player1.health);
            prior_player_health = player1.health;
        }
        if(game_status == 1){
            break;
        }

        human *characters[] = {&player1, &npc1, &npc2, &npc3};   // Write only
        human characters2[] = {*characters[0],*characters[1], *characters[2], *characters[3] };  // Read only

        char c = getUart();

        *characters[0] = controlCharater(map,characters2, *characters[0], c, tracking_player_on_map(*characters[0], map, c),&got_hit_player, mage_walking_allArray);
        
        character_take_damage(&characters,&got_hit_player,&take_damaged_once,4);
        

        int timer = set_wait_timer(0, 0);
        if (timer)
        {   
            for(int i= 1; i < 4; i++){  // plan best route for all npcs
                if(timer % npc_timer[i-1] == 0){
                    if((player1.x != player_prior_x[i-1] || player1.y != player_prior_y[i-1] ) && characters[i]->is_alive){
                    player_prior_x[i-1] = player1.x;
                    player_prior_y[i-1] = player1.y;
                    drawRectARGB32(characters[i]->x, characters[i]->y, characters[i]->x + characters[i]->frame_width, characters[i]->y + characters[i]->frame_height, 0x00000000, 1);
                    characters[i]->x = (characters[i]->x/block_width) * block_width;
                    characters[i]->y = (characters[i]->y/block_height) * block_height;
                    reset_flood_map(map);
                    dijkstra_find_route(flood_map, characters[i]->y/block_height,characters[i]->x/block_width,player1.y/block_height,player1.x/block_width,distances);
                    dijkstra_plan_route(flood_map,characters[i]->y/block_height,characters[i]->x/block_width,player1.y/block_height,player1.x/block_width,distances,npc_chases[i-1]);
                    characters[i]->move_index = 0;
                    }
                }
            }



            // Player hit dection
            if((timer % 50) == 0){ // every 500ms
                if(npc_hit_detection(characters2, player1.x,player1.y)){
                    player1.offset = 36;
                    player1.health -=1;
                    if(player1.health < 0){
                        //player1.offset = 43;
                        //controlCharater(map,characters2, *characters[0], 'p', tracking_player_on_map(*characters[0], map, 'p'),&got_hit_player, mage_walking_allArray);
                        player1.is_alive =0;
                        game_status = 1;
                    }
                }
            }



            if ((timer % 10) == 0)  // animation update every 100ms
            {   
                for(int i = 0; i< 4; i++){
                    if(characters[i]->got_hit == 1){
                        //*characters[i] = controlCharater(map,characters2, *characters[i],'t', 0, &got_hit_player,mage_walking_allArray);
                        characters[i]->offset = characters[i]->movedown_frame_offset;
                        characters[i]->got_hit = 0;
                        // uart_dec(characters[i]->health);
                        // uart_sendc('\n');
                        if(characters[i]->health == 0){
                            characters[i]->is_alive = 0;
                        }

                    }

                }
                npc1 = move(map, characters2, npc1, death_npc_chase, 100, 0,&got_hit_player,death_allArray);
                npc2 = move(map, characters2, npc2, goblem_npc_chase, 100, 0,&got_hit_player,goblem_allArray);
                npc3 = move(map, characters2, npc3, red_dude_npc_chase, 100, 0,&got_hit_player,red_dude_allArray);



                // For bomb animation
                for (int i = 0; i < player1.bomb_num; i++)
                {
                    player1.bomb[i].delay++;
                    if (player1.bomb[i].delay > 20)
                    { // delay 2 seconds
                        player1.bomb[i].frame++;
                        if (player1.bomb[i].frame > 11)
                        {   
                            if(player1.bomb[i].state){
                                take_damaged_once = 1;
                            }
                            player1.bomb[i].frame = 0;
                            player1.bomb[i].state = 0;
                            player1.bomb[i].delay = 0;
                        }
                    }
                }
            }

            set_wait_timer(1, 10000); // reset 10ms timer
        }
    }
}

void sub_boss(){
    
    draw_map_from_array(map4);
    human player1 = character1_init(38, 46 * 3, 9,0,8,mage_width,mage_height);
    human knight1 = character1_init(38 * 3, 46 * 5, 9,1,8,59,72);
    human knight2 = character1_init(38 * 3, 46 * 10, 9,1,8,59,72);
    human knight3 = character1_init(38 * 23, 46 * 5, 9,1,8,59,72);
    human knight4 = character1_init(38 * 23, 46 * 10, 9,1,8,59,72);

    set_wait_timer(1, 10000); // set 10ms

    int knight_timer[] = {10,20,25,30};
    int counter[] = {0,0,0,0};
    int move_signal[] = {0,0,0,0};
    int player_prior_x[] = {knight1.x,knight2.x,knight3.x, knight4.x};
    int player_prior_y[] = {knight1.y, knight2.y,knight3.y, knight4.y};
    int prior_player_health = player1.health;
    int game_status = 0;
    draw_stats(5);
    while (1)
    {   
        if(prior_player_health != player1.health){
            drawRectARGB32(0,0,500,50,0x00000000,1);
            draw_stats(player1.health);
            prior_player_health = player1.health;
        }

        if(game_status == 1){
            break;
        }
        

        human *characters[] = {&player1,&knight1,&knight2,&knight3,&knight4};   // Write only
        human characters2[] = {*characters[0],*characters[1],*characters[2], *characters[3], *characters[4]};  // Read only

        char c = getUart();

        *characters[0] = controlCharater(map4,characters2, *characters[0], c, tracking_player_on_map(*characters[0], map4, c),&got_hit_player, mage_walking_allArray);

        character_take_damage(&characters,&got_hit_player,&take_damaged_once,5);
        
        
        
        int timer = set_wait_timer(0, 0);
        if (timer)
        {
            if ((timer % 10) == 0)
            {    
                //uart_dec(move_signal[0]);
                //uart_sendc('\n');
                for(int i = 0; i< 4; i++){
                    if(characters[i+1]->got_hit){
                        move_signal[i] = 4;   // move to huting phase
                        characters[i+1]->got_hit = 0;
                        if(characters[i+1]->health <= 0){
                            move_signal[i] = 3;   // move to dying phase
                        }
                    }

                }
                
                for(int i = 0; i < 4; i++){
                    if(move_signal[i] != 3){        // Take hit phase
                        counter[i]++;
                        if(counter[i] == knight_timer[0] && move_signal[i] == 2){ // attack phase
                            if(npc_hit_detection(characters2, player1.x,player1.y)){
                                //*characters[0] = controlCharater(map4,characters2, *characters[0], 't', tracking_player_on_map(*characters[0], map4, 't'),&got_hit_player, mage_walking_allArray);
                                player1.offset = 36;
                                player1.health -=1;
                                if(player1.health < 0){
                                    player1.is_alive =0;
                                    game_status = 1;
                                }
                            }
                            if(player1.x != player_prior_x[i] || player1.y != player_prior_y[i]){
                                move_signal[i] = 0;
                            }
                            counter[i] = 0;
                        }else if(counter[i] == knight_timer[i]){        // moving phase
                            player_prior_x[i] = player1.x;
                            player_prior_y[i] = player1.y;
                            move_signal[i] = 1;
                            counter[i] = 0;
                        }
                    }
                    
                }
            
                knight1 = control_knight(player1, knight1,&move_signal[0],player_prior_x[0],player_prior_y[0],5);
                knight2 = control_knight(player1, knight2,&move_signal[1],player_prior_x[1],player_prior_y[1],7);
                knight3 = control_knight(player1, knight3,&move_signal[2],player_prior_x[2],player_prior_y[2],10);
                knight4 = control_knight(player1, knight4,&move_signal[3],player_prior_x[3],player_prior_y[3],8);

                for (int i = 0; i < player1.bomb_num; i++)
                {
                    player1.bomb[i].delay++;
                    if (player1.bomb[i].delay > 20)
                    { // delay 2 seconds
                        player1.bomb[i].frame++;
                        if (player1.bomb[i].frame > 11)
                        {   
                            if(player1.bomb[i].state){
                                take_damaged_once = 1;
                            }
                            player1.bomb[i].frame = 0;
                            player1.bomb[i].state = 0;
                            player1.bomb[i].delay = 0;
                        }
                    }
                }

            }

            set_wait_timer(1, 10000); // reset 10ms timer
        }
    }
}

void final_boss(){
    int i = 0;
    set_wait_timer(1, 10000); // set 10ms
    while (1)
    {   
        int timer = set_wait_timer(0, 0);
        if (timer)
        {
            if ((timer % 10) == 0)
            { 
                
                drawGameAsset(i,500 , 500 , 32,32, death_allArray);
                i++;
                /*
                drawGameAsset(i,400,200,dragon_boss_width,dragon_boss_height,dargon_boss_allArray);
                i++;
                if(i > 7){
                    i = 0;
                }
                */
                if(i > death_allArray_LEN-1){
                    i = 0;
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
    // echo everything back
    
    human player1 = character1_init(38, 46 * 3, 9,0,8,mage_width,mage_height);
    human npc1 = character1_init(38*1, 46 * 11, 9,0,8,mage_width,mage_height);

    //dijkstra_find_route(flood_map, 11,1,3,1,distances);
    //dijkstra_plan_route(flood_map,11,1,3,1,distances,npc_chase);
    
    //flood_fill(flood_map, 11, 3,0);
    //plan_routes(npc1, &npc_chase);
    //print_map(flood_map);
    
    //play_game(map2);
    play_game(map3);
    //final_boss();
    //sub_boss();
    //goblin_test();
    uart_puts("GAME OVER\n");
}