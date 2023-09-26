
#include "../uart/uart1.h"
#include "framebf.h"
#include "map.h"
#include "./player/magewalking.h"
#include "./sub_boss/knight.h"      // character.h here
#include "map_array.h"
#include "./npcs/goblin.h"
#include "./npcs/red_dude.h"
#include "./npcs/goblem.h"
#include "./npcs/death.h"
#include "./npcs/gladiatorwalking.h"
#include "./npcs/mush_room.h"
#include "./npcs/eye_ball.h"
#include "./welcome_image.h"

// the compiler randomly ask for memcpy,memset so i included it here
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


int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

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
            distances[i][j] = 2147483647;   // set to max integer
        }
    }
}

void dijkstra_find_route(int map[][28], int x, int y, int x_end, int y_end, int distance[][28]){
    if(map[x_end][y_end] != -2){ // if not a road return
        return;
    }
    if(x == x_end && y == y_end){   // if reach destination return
        return;
    }

    for(int i =0; i < 4; i++){  // Check all 4 direction
        int next_move[] = {x + dx[i], y + dy[i]};

        if(next_move[0] < 17 && next_move[1] < 28 && map[next_move[0]][next_move[1]] != -1){
            int newDistant = distance[x][y] +1; // turn to interger min value due to integer overflow

            if(newDistant < distance[next_move[0]][next_move[1]]){
                distance[next_move[0]][next_move[1]] = newDistant;
                dijkstra_find_route(map,next_move[0], next_move[1],x_end,y_end, distance);
            }
        }
    }
}

void dijkstra_plan_route(int map[][28], int x, int y,int x_end, int y_end, int distance[][28], moves *npc_chase){
    if(map[x_end][y_end] != -2){   // if not road return
        return;
    }
    
    int current_x = x_end;
    int current_y = y_end;

    while(current_x != x || current_y != y){
        if((current_x == x -1 && current_y == y) || (current_x == x + 1 && current_y == y) || (current_x == x && current_y == y-1) ||(current_x == x && current_y == y+1) ){
            break;  // if arrive near player stop
        }
        
        for(int i = 0; i < 4; i++){
            int next_move[] = {current_x + dx[i], current_y + dy[i]};
            if(next_move[0] < 17 && next_move[1] < 28 && distance[next_move[0]][next_move[1]] == distance[current_x][current_y] -1){
                
                current_x = next_move[0];
                current_y = next_move[1];
                map[current_x][current_y] = 8;  // mark fastest route as 8
                break;
            }
        }
    }

    int index = 0;
    
    current_x = x;
    current_y = y;
    while(1){
        int min_index = 0;
        if((current_x == x_end -1 && current_y == y_end) || (current_x == x_end + 1 && current_y == y_end) || (current_x == x_end && current_y == y_end-1) ||(current_x == x_end && current_y == y_end+1) ){
            break;  // if arrived near player stop
        }
        for(int i = 0; i< 4; i++){
            int next_move[] = {current_x + dx[i], current_y + dy[i]};
            if(map[next_move[0]][next_move[1]] == 8){
                map[next_move[0]][next_move[1]] = 0; // mark as travelled
                min_index = i;
            }

        }

        // compute moves for npc
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

human play_game1(int map[][28]){
    draw_map_from_array(map);

    ////////////////////////////
    // NPC, Player init
    moves npc1_moves[] = {
    {'w', {1,7}},
    {'s', {1,11}}};

    moves npc2_moves[] = {
        {'d', {8,9}},
        {'a', {5,9}}};

     moves npc3_moves[] = {
        {'a', {10,3}},
        {'d', {14,3}}};

    moves npc4_moves[] = {
    {'d', {20,3}},
    {'a', {17,3}}};

    moves npc5_moves[] = {
    {'s', {17,11}},
    {'w', {17,9}}};

    moves npc6_moves[] = {
    {'w', {23,6}},
    {'s', {23,10}}};
    

    // wall block width and height is 38 and 46
    // span characters in the below format
    human player1 = character1_init(block_width*1, block_height * 3, 9,0,8,mage_width,mage_height,5);

    human npc1 = character1_init(block_width * 1, block_height * 11, 7,1,6,goblin_width,goblin_height,1);
    human npc2 = character1_init(block_width * 5, block_height * 9, 4,1,3,gladiator_width,gladiator_height,1);
    human npc3 = character1_init(block_width * 14, block_height * 3, 3,1,2,mush_room_width,mush_room_height,1);
    human npc4 = character1_init(block_width * 17, block_height * 3, 3,1,2,eye_ball_width,eye_ball_height,1);
    human npc5 = character1_init(block_width * 17, block_height * 9, 3,1,2,red_dude_width,red_dude_height,1);
    human npc6 = character1_init(block_width * 23, block_height * 10, 3,1,2,mush_room_width,mush_room_height,1);
    //////////////////////////////////
    

    // Keep track of previous stat
    int prior_player_health = player1.health;
    int prior_player_bomb_damage = player1.bomb_damage;
    int prior_player_bomb_range = player1.bomb_range;
    int game_status = 0;

    // draw stat
    draw_stats(player1.health, player1.bomb_damage, player1.bomb_range);

    // 1 seconds = 1000000
    set_wait_timer(1, 10000); // set 10ms
    while (1)
    {   
        //update new stat if there is changes
        if(player1.x/block_width == 25 && player1.y/block_height == 17){
            return player1;
        }
        char c = getUart();
        //update new stat if there is changes
        if(prior_player_health != player1.health || prior_player_bomb_damage != player1.bomb_damage || prior_player_bomb_range != player1.bomb_range){
            drawRectARGB32(0,0,500,50,0x00000000,1);
            draw_stats(player1.health, player1.bomb_damage, player1.bomb_range);
            prior_player_health = player1.health;
            prior_player_bomb_damage = player1.bomb_damage;
            prior_player_bomb_range = player1.bomb_range;
        }
        if(game_status == 1){   // break if false game
            break;
        }

        human *characters[] = {&player1, &npc1, &npc2, &npc3,&npc4,&npc5,&npc6};   // Write only
        human characters2[] = {*characters[0],*characters[1], *characters[2], *characters[3], *characters[4], *characters[5],*characters[6]};  // Read only


        *characters[0] = controlCharater(map,characters2, *characters[0], c, tracking_player_on_map(*characters[0], map, c),&got_hit_player, mage_walking_allArray);
        
        // got_hit_player is npc got hit by the bomb, update their health if hit
        character_take_damage(&characters,&got_hit_player,&take_damaged_once,7); 
        

        int timer = set_wait_timer(0, 0);
        if (timer)
        {   
            // Player collision between characters
            if((timer % 50) == 0){ // every 500ms
                if(npc_hit_detection(characters2, player1.x,player1.y) == 2){
                    player1.offset = 36;
                    player1.health -=1;
                    if(player1.health < 0){
                        player1.is_alive =0;
                        game_status = 1;
                    }
                }
            }

            if ((timer % 10) == 0)  // animation update every 100ms
            {   
                // Check bomb hit for all characters
                for(int i = 0; i< 7; i++){
                    if(characters[i]->got_hit == 1){
                        characters[i]->got_hit = 0;
                        if(characters[i]->health == 0){
                            characters[i]->is_alive = 0;
                        }
                    }
                }
                

                // Move npcs
                npc1 = move(map, characters2, npc1, npc1_moves, 2,&got_hit_player,goblin_walking_allArray,1);
                npc2 = move(map, characters2, npc2, npc2_moves, 2,&got_hit_player,gladiator_walking_allArray,1);
                npc3 = move(map, characters2, npc3, npc3_moves, 2,&got_hit_player,mush_room_allArray,1);
                npc4 = move(map, characters2, npc4, npc4_moves, 2,&got_hit_player,eye_ball_allArray,1);
                npc5 = move(map, characters2, npc5, npc5_moves, 2,&got_hit_player,red_dude_allArray,1);
                npc6 = move(map, characters2, npc6, npc6_moves, 2,&got_hit_player,mush_room_allArray,1);

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

human play_game2(int map[][28], human player1)
{   
    // span player top left
    player1.x = block_width*1;
    player1.y = block_height*3;
    drawRectARGB32(0,0,1024,768,0x00000000,1); // clear screen
    draw_map_from_array(map);


    // list int all item type and their coordinate
    Items item1 = {1,13,'r',1};
    Items item2 = {23,13,'h',1};
    Items item3 = {25,3,'d',1};


    Items items[] = {item1,item2,item3}; 

    moves death_npc_chase[100];
    moves red_dude_npc_chase[100];
    moves goblem_npc_chase[100];
    moves mush_room_chase[100];

    moves *npc_chases[] = {&death_npc_chase, &goblem_npc_chase, &red_dude_npc_chase, &mush_room_chase};
    const unsigned long **npc_array[] = {death_allArray,goblem_allArray,red_dude_allArray, mush_room_allArray};

    // wall block width and height is 38 and 46
    // span characters in the below format

    human npc1 = character1_init(block_width * 1, block_height * 14, 3,1,2,death_width,death_height,5);
    human npc2 = character1_init(block_width * 8, block_height * 9, 3,1,2,goblem_width,goblem_height,5);
    human npc3 = character1_init(block_width * 15, block_height * 3, 3,1,2,red_dude_width,red_dude_height,5);
    human npc4 = character1_init(block_width * 8, block_height * 3, 3,1,2,mush_room_width,mush_room_height,5);
    
    //print_map(flood_map);

    // Keep track of previous stat
    int prior_player_health = player1.health;
    int prior_player_bomb_damage = player1.bomb_damage;
    int prior_player_bomb_range = player1.bomb_range;
    int game_status = 0;

    draw_stats(player1.health, player1.bomb_damage, player1.bomb_range);

    int player_prior_x[] = {0,0,0,0};
    int player_prior_y[] = {0, 0,0,0};
    int npc_timer[] = {150,200,250,300};
    
    // 1 seconds = 1000000
    set_wait_timer(1, 10000); // set 10ms
    while (1)
    {   
        char c = getUart();
        //update new stat if there is changes
        if(prior_player_health != player1.health || prior_player_bomb_damage != player1.bomb_damage || prior_player_bomb_range != player1.bomb_range){
            drawRectARGB32(0,0,500,50,0x00000000,1);
            draw_stats(player1.health, player1.bomb_damage, player1.bomb_range);
            prior_player_health = player1.health;
            prior_player_bomb_damage = player1.bomb_damage;
            prior_player_bomb_range = player1.bomb_range;
        }

        if(game_status == 1){
            player1.is_alive = 1;
            return player1;
        }else if(game_status == 2){
            player1.is_alive = 0;
            return player1;
        }

        human *characters[] = {&player1, &npc1, &npc2, &npc3,&npc4};   // Write only
        human characters2[] = {*characters[0],*characters[1], *characters[2], *characters[3],*characters[4] };  // Read only


        *characters[0] = controlCharater(map,characters2, *characters[0], c, tracking_player_on_map(*characters[0], map, c),&got_hit_player, mage_walking_allArray);
        
        // got_hit_player is npc got hit by the bomb, update their health if hit
        character_take_damage(&characters,&got_hit_player,&take_damaged_once,5);
        
        game_status = 1;
        for(int i = 1; i < 5; i++){
            if(characters[i]->is_alive == 1){
                game_status = 0;
            }
        }

        // Get items for player
        for(int i = 0; i < sizeof(items)/sizeof(items[0]); i++){
            if(player1.x/block_width == items[i].x && player1.y/block_height == items[i].y){
                if(items[i].available == 0){
                    continue;
                }
                items[i].available = 0;
                if(items[i].type == 'r'){
                    player1.bomb_range++;
                    uart_puts("range item\n");
                }else if(items[i].type == 'h'){
                    player1.health++;
                    uart_puts("health item\n");
                }else if(items[i].type == 'd'){
                    player1.bomb_damage+=2;
                    uart_puts("damage item\n");
                }
            }
        }

        int timer = set_wait_timer(0, 0);
        if (timer)
        {   
            for(int i= 1; i < 5; i++){  // plan best route for all npcs
                if(timer % npc_timer[i-1] == 0){
                    if((player1.x != player_prior_x[i-1] || player1.y != player_prior_y[i-1] ) && characters[i]->is_alive){
                    player_prior_x[i-1] = player1.x;    // update player coordinate for each npc at different time
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
                        player1.is_alive = 0;
                        game_status = 2;
                    }
                }
            }

            for(int i = 1; i< 5; i++){  // NPC take damge
                    if(characters[i]->got_hit == 1){
                        *characters[i] = controlCharater(map,characters2, *characters[i], 'h', tracking_player_on_map(*characters[i], map, 'h'),&got_hit_player, npc_array[i-1]);
                        characters[i]->got_hit = 0;
                        if(characters[i]->health <= 0){
                            characters[i]->is_alive = 0;
                        }

                    }
                }

            if ((timer % 10) == 0)  // animation update every 100ms
            {   
                for(int i = 1; i < 5; i++){
                    *characters[i] = move(map, characters2, *characters[i], npc_chases[i-1], 100,&got_hit_player,npc_array[i-1],0);
                }

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

int final_boss(human player1){
    player1.x = 500;
    player1.y = 500;
    //player1.bomb_damage = 5;
    
    drawRectARGB32(0,0,1024,768,0x00000000,1); // clear screen
    draw_map_from_array(map4);

    //human player1 = character1_init(38, 46 * 3, 9,0,8,mage_width,mage_height);
    human dragon_boss = character1_init(block_width * 10, block_height * 5, 7,1,6, 168, 175,10);
    Explosion explosion = {dragon_boss.x+80,dragon_boss.y+80,dragon_boss.x+80,dragon_boss.y+80,0,0,0};

    human knight1 = character1_init(block_width * 3, block_height * 5, 9,1,8,59,72, 5);
    human knight2 = character1_init(block_width * 3, block_height * 10, 9,1,8,59,72,5);
    human knight3 = character1_init(block_width * 23, block_height * 5, 9,1,8,59,72,5);
    human knight4 = character1_init(block_width * 23, block_height * 10, 9,1,8,59,72,5);

    set_wait_timer(1, 10000); // set 10ms

    int knight_timer[] = {10,15,20,25,30,30,30,30,30};
    int counter[] = {0,0,0,0,0,0,0,0,0};
    int move_signal[] = {0,0,0,0};
    int player_prior_x[10];
    int player_prior_y[10];
    for(int i = 0; i < 10; i++){
        player_prior_x[i] = player1.x ;
        player_prior_y[i] = player1.y;
    }

    // save previous stat
    int prior_player_health = player1.health;
    int prior_player_bomb_damage = player1.bomb_damage;
    int prior_player_bomb_range = player1.bomb_range;
    int prior_dragon_boss_health = dragon_boss.health;

    int game_status = 0;
    
    draw_boss_stats(dragon_boss.health);
    draw_stats(player1.health, player1.bomb_damage, player1.bomb_range);

    while (1)
    {   
        char c = getUart();
        // update if stats changed
        if(prior_dragon_boss_health != dragon_boss.health){
            draw_boss_stats(dragon_boss.health);
            prior_dragon_boss_health = dragon_boss.health;
        }
        // update if stats changed
        if(prior_player_health != player1.health || prior_player_bomb_damage != player1.bomb_damage || prior_player_bomb_range != player1.bomb_range){
            draw_stats(player1.health, player1.bomb_damage, player1.bomb_range);
            prior_player_health = player1.health;
            prior_player_bomb_damage = player1.bomb_damage;
            prior_player_bomb_range = player1.bomb_range;
        }

        if(game_status == 1){
            return 1;   // victory
        }else if(game_status == 2){
            return 2;   // loose
        }

        human *characters[] = {&player1,&knight1,&knight2,&knight3, &knight4, &dragon_boss};   // Write only
        human characters2[] = {*characters[0],*characters[1],*characters[2], *characters[3], *characters[4], *characters[5]};  // Read only


        *characters[0] = controlCharater(map4,characters2, *characters[0], c, tracking_player_on_map(*characters[0], map4, c),&got_hit_player, mage_walking_allArray);
        // got_hit_player is npc got hit by the bomb, update their health if hit
        character_take_damage(&characters,&got_hit_player,&take_damaged_once,6);
        
        game_status = 1;
        for(int i = 1; i < 6; i++){
            if(characters[i]->is_alive == 1){
                game_status = 0;
            }
        }
        
        
        int timer = set_wait_timer(0, 0);
        if (timer)
        {   
            // if player die escape
            if(player1.health <= 0){
                player1.is_alive = 0;
                game_status = 2;
            }
            if(timer % 100 == 0){   // check npc and player collide every seconds
                if(npc_hit_detection(characters2, player1.x,player1.y) == 1){
                    player1.health--;
                    
                }                
            }
            if ((timer % 10) == 0)
            {    
                for(int i = 0; i < 5; i++){ // update player coordinate for each npc at different time
                    counter[i+4]++;
                    if(counter[i+4] == knight_timer[i+4]){
                        player_prior_x[i+4] = player1.x;
                        player_prior_y[i+4] = player1.y;
                        counter[i+4] = 0;
                    }
                }

                for(int i = 0; i< 5; i++){
                    if(characters[i+1]->got_hit){
                        move_signal[i] = 4;   // move to huting phase
                        characters[i+1]->got_hit = 0;
                        if(characters[i+1]->health < 0){
                            if(i+1 > 4){
                                characters[i+1]->is_alive = 0;
                            }
                            move_signal[i] = 3;   // move to dying phase
                        }
                    }

                }
                
                for(int i = 0; i < 4; i++){
                    if(move_signal[i] != 3){        // Take hit phase
                        counter[i]++;
                        if(counter[i] == knight_timer[i] && move_signal[i] == 2){ // attack phase
                            if(npc_hit_detection(characters2, player1.x,player1.y)){
                                player1.offset = 36;
                                player1.health -=1;
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

                dragon_boss = control_dragon(&player1,dragon_boss,&explosion, player_prior_x[4], player_prior_y[4],20);
                if(dragon_boss.health < 5){
                     knight1 = control_knight(player1, knight1,&move_signal[0],player_prior_x[0],player_prior_y[0],5);
                    knight2 = control_knight(player1, knight2,&move_signal[1],player_prior_x[1],player_prior_y[1],7);
                    knight3 = control_knight(player1, knight3,&move_signal[2],player_prior_x[2],player_prior_y[2],9);
                    knight4 = control_knight(player1, knight4,&move_signal[3],player_prior_x[3],player_prior_y[3],8);
                }

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

void promt_continue(){
        while(1){
        char c = getUart();
        if(c == 'p'){
            drawRectARGB32(0,0,1024,768,0x00000000,1); // clear screen
            break;
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
    drawGameAsset(0,150,0,welcome_page_width, welcome_page_height,welcome_page_allArray);

    promt_continue();
    drawString(180,200, "In a world veiled in darkness and peril, an unsung hero emerges. Their quest: to unlock the ", 0x0d);
    drawString(180,200+20, "power of ancient relics and avert impending catastrophe. The journey begins with a ", 0x0d);
    drawString(180,200+20*2, "treacherous maze, guarded by monstrous foes.", 0x0d);
    drawString(180,200+20*4, "Armed with alchemical expertise and a deadly arsenal of bombs, our hero strides forth. They", 0x0d);
    drawString(180,200+20*5, "must conquer the labyrinth, decipher secrets, and face formidable adversaries. With AWDS", 0x0d);
    drawString(180,200+20*6, "keys for movement and explosive prowess bound to 'J', their fate unfolds with each step.", 0x0d);
    drawString(180,200+20*8, "As the adventure ignites, the hero's footsteps echo through the dark corridors, and the ", 0x0d);
    drawString(180,200+20*9, "destiny of the world hangs in the balance. The legend unfolds in a symphony of courage and ", 0x0d);
    drawString(180,200+20*10, "explosions.", 0x0d);
    drawString(450,200+20*12, "Press P to continue", 0x0d);
    promt_continue();

    human temp = play_game1(map2);
    if(temp.is_alive == 0){
        draw_game_status(2);
        uart_puts("GAME OVER\n");
        return;
    }
    drawRectARGB32(0,0,1024,768,0x00000000,1); // clear screen
    drawString(180,200, "Beyond the labyrinth's first trials, a new threat looms – relentless monsters hot on our hero's ", 0x0d);
    drawString(180,200+20, "heels. Escape is paramount. Scattered throughout the maze lie essential items, the keys to ", 0x0d);
    drawString(180,200+20*2, "survival", 0x0d);
    drawString(180,200+20*4, "As the chase intensifies, our hero races against time, snatching vital items while evading ", 0x0d);
    drawString(180,200+20*5, "their relentless pursuers. Each step is a heartbeat away from peril, and every choice carries ", 0x0d);
    drawString(180,200+20*6, "life-or-death consequences.", 0x0d);
    drawString(180,200+20*8, "In this heart-pounding stage, the labyrinth becomes a deadly game of wits and speed. The", 0x0d);
    drawString(180,200+20*9, "hero's fate hinges on their ability to run, collect, evade, and conquer. The legend continues in ", 0x0d);
    drawString(180,200+20*10, "a breathless race for survival.", 0x0d);
    drawString(450,200+20*12, "Press P to continue", 0x0d);
    promt_continue();

    //human temp = character1_init(block_width*1, block_height * 3, 9,0,8,mage_width,mage_height,5);
    human temp2 = play_game2(map3,temp);
    if(temp2.is_alive == 0){
        draw_game_status(2);
        uart_puts("GAME OVER\n");
        return;
    }
    drawRectARGB32(0,0,1024,768,0x00000000,1); // clear screen
    drawString(180,200, "At the heart of the labyrinth, where darkness and danger converge, our hero's journey ", 0x0d);
    drawString(180,200+20, "reaches its climax. A formidable adversary awaits – a powerful dragon, a guardian of ancient ", 0x0d);
    drawString(180,200+20*2, "secrets, capable of spewing deadly fireballs.", 0x0d);
    drawString(180,200+20*4, "The air is thick with tension, for this is no ordinary battle. The dragon's fiery onslaught ", 0x0d);
    drawString(180,200+20*5, "demands agility and precision. Every move, every breath, must be calculated to perfection.", 0x0d);
    drawString(180,200+20*7, "life-or-death consequences.", 0x0d);
    drawString(180,200+20*8, "But the dragon is not the only threat. The labyrinth, relentless in its malevolence, springs ", 0x0d);
    drawString(180,200+20*9, "unexpected ambushes. The hero must remain vigilant, for danger lurks around every corner.", 0x0d);
    drawString(180,200+20*11, "As the hero steps into this fiery arena, the legend draws to its epic conclusion. In the face of ", 0x0d);
    drawString(180,200+20*12, "searing flames and treacherous surprises, their courage and determination will be put to the ", 0x0d);
    drawString(180,200+20*13, "ultimate test. The final stage has been set, and the hero's destiny hangs in the balance, ", 0x0d);
    drawString(180,200+20*14, "poised to unravel in a blaze of fire and suspense.", 0x0d);
    drawString(450,200+20*16, "Press P to continue", 0x0d);
    promt_continue();

    //human temp2 = character1_init(block_width*1, block_height * 3, 9,0,8,mage_width,mage_height,5);
    int result = final_boss(temp2);
    if(result == 1){
        draw_game_status(1);
        uart_puts("VICTORY");
    }else{
        draw_game_status(2);
        uart_puts("GAME OVER\n");
    }
}