
#include "character.h"
#include "bomb.h"
#include "framebf.h"
#include "bomb_explosion.h"
#include "../uart/uart1.h"

void character_take_damage(human *characters[], int *got_hit_player,int *take_damaged_once,int size){
        for(int i = 0; i <size; i++){
            if(*take_damaged_once == 1){
                if(*got_hit_player == i){
                    if(i == 0){
                        characters[i]->offset = 36;
                    }
                    characters[i]->health -= characters[0]->bomb_damage;
                    //if(characters[i]->health < 0 ){
                        //characters[i]->is_alive = 0;
                    //}
                    characters[i]->got_hit = 1;
                    *got_hit_player = -1;
                    *take_damaged_once = 0;
                }
            }
        }
}

int wall_ignore = 5;
// Track player on the map and collison dection with walls

unsigned int absolute(int num)
{
    if (num < 0)
    {
        num = num * -1;
        return num;
    }
    return num;
}

void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char* int_to_string(int num, char* str, int base) {
    int i = 0;
    int is_negative = 0;

    // Handle 0 explicitly
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Handle negative numbers for base 10
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }

    // Convert the number to a string
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem < 10) ? rem + '0' : rem + 'a' - 10;
        num = num / base;
    }

    // Append '-' for negative numbers
    if (is_negative)
        str[i++] = '-';

    str[i] = '\0';

    // Reverse the string
    reverse(str, i);

    return str;
}


int tracking_player_on_map(human player, int map[][28], char c)
{
    int player_width = player.frame_width;
    int player_height = player.frame_height;
    int clearance_y = 5;
    int clearance_x = 5;
    int bouding_box[][3] = {{player.y - clearance_y, player.x ,player.x + player_width + clearance_x-2,},
                            {player.y + player_height + clearance_y, player.x, player.x + player_width+ clearance_x-2},
                            {player.y, player.y + player_height + clearance_y-6, player.x - clearance_x},
                            {player.y, player.y + player_height + clearance_y-6, player.x + player_width + clearance_x+3}}; 

    if (c == 'a')
    {   
        if( (map[bouding_box[2][0] / 46][bouding_box[2][2] / 38] > 0 || map[bouding_box[2][1] / 46][(player.x - clearance_x) / 38] > 0)
            && (map[bouding_box[2][0] / 46][bouding_box[2][2] / 38] < wall_ignore || map[bouding_box[2][1] / 46][(player.x - clearance_x) / 38] < wall_ignore ))
        {
            return 1;
        }
    }
    else if (c == 'd')
    {
        if ((map[bouding_box[3][0] / 46][bouding_box[3][2] / 38] > 0 || (map[bouding_box[3][1] / 46][bouding_box[3][2] / 38] > 0))
            && (map[bouding_box[3][0] / 46][bouding_box[3][2] / 38] < wall_ignore || (map[bouding_box[3][1] / 46][bouding_box[3][2] / 38] < wall_ignore)))
        {
            return 1;
        }
    }
    else if (c == 'w')
    {
        if ((map[bouding_box[0][0]/ 46][bouding_box[0][1] / 38] > 0 || map[bouding_box[0][0] / 46][bouding_box[0][2] / 38] > 0)
            && (map[bouding_box[0][0]/ 46][bouding_box[0][1] / 38] < wall_ignore || map[bouding_box[0][0] / 46][bouding_box[0][2] / 38] < wall_ignore))
        {
            return 1;
        }
    }
    else if (c == 's')
    {
        if ((map[bouding_box[1][0] / 46][bouding_box[1][1] / 38] > 0 || map[bouding_box[1][0] / 46][bouding_box[1][2] / 38] > 0)
            && (map[bouding_box[1][0] / 46][bouding_box[1][1] / 38] < wall_ignore || map[bouding_box[1][0] / 46][bouding_box[1][2] / 38] < wall_ignore))
        {
            return 1;
        }
    }

    return 0;
}

int npc_hit_detection(human humans[], unsigned int object_x, unsigned int object_y){

   int player_bounding_box[][2] = {{object_y, object_x},
                        {object_y, object_x + humans[0].frame_width-1},
                        {object_y + humans[0].frame_height-1, object_x},
                        {object_y + humans[0].frame_height-1, object_x + humans[0].frame_width-1}};


    // collision with npc
    for(int i = 1; i < 10; i++){
        for(int j = 0; j < 4; j++){
            int character_bounding_box[][2] = {{humans[i].x, humans[i].y},
                                            {humans[i].x + humans[i].frame_width, humans[i].y},
                                            {humans[i].x, humans[i].y + humans[i].frame_height},
                                            {humans[i].x + humans[i].frame_width, humans[i].y + humans[i].frame_height}};
            
            if(i > 4){
                            // collison check for 4 corner
                if(player_bounding_box[1][1] >= character_bounding_box[0][0]
                    && player_bounding_box[0][1] <= character_bounding_box[1][0]
                    && player_bounding_box[2][0] >= character_bounding_box[0][1]
                    && player_bounding_box[0][0] <= character_bounding_box[2][1]){
                        return 1;
                    }
            }

            // false safe collision for smaller character
                if(absolute(object_x - humans[i].x) < 42 && absolute(object_y - humans[i].y) < 49){
                    return 2;
                }
        }
    }
    
    return 0;
}

int collision_detection(int map[][28],human humans[], unsigned int object_x, unsigned int object_y){
    
    int bomb_bounding_box[][2] = {{object_y, object_x},
                        {object_y, object_x + explosion_width-1},
                        {object_y + explosion_height-1, object_x},
                        {object_y + explosion_height-1, object_x + explosion_width-1}};

        // collision with walls
    for(int i = 0; i < 4; i++){
        if(map[bomb_bounding_box[i][0]/46][bomb_bounding_box[i][1]/38] > 0){
            return -1;
        }
    }
    
    // collision with npc
    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 4; j++){
            int character_bounding_box[][2] = {{humans[i].x, humans[i].y},
                                            {humans[i].x + humans[i].frame_width, humans[i].y},
                                            {humans[i].x, humans[i].y + humans[i].frame_height},
                                            {humans[i].x + humans[i].frame_width, humans[i].y + humans[i].frame_height}};
            

            // collison check for 4 corner
            if(bomb_bounding_box[1][1] >= character_bounding_box[0][0]
                && bomb_bounding_box[0][1] <= character_bounding_box[1][0]
                && bomb_bounding_box[2][0] >= character_bounding_box[0][1]
                && bomb_bounding_box[0][0] <= character_bounding_box[2][1]){
                    return i;
                }

            // false safe collision for smaller character
            for(int k = 0; k < 4; k++){
                if(absolute(bomb_bounding_box[k][1] - character_bounding_box[j][0]) < 10 && absolute(bomb_bounding_box[k][0] - character_bounding_box[j][1]) < 10){
                    return i;
                }
            }
        }
    }
    return -2;
}

void drawGameAsset(int frame, unsigned int offset_x, unsigned int offset_y, unsigned int width, unsigned int height, const unsigned long *frame_array[])
{

    int index = 0;
    for (int i = 0 + offset_x; i < width + offset_x; i++)
    {
        for (int j = 0 + offset_y; j < height + offset_y; j++)
        {
           
            drawPixelARGB32(i, j, frame_array[frame][index]);
            index++;
        }
    }
}


human plant_bomb(int map[][28],human characters[], human player1, char c, int *hit_player)
{
    if (c == 'j')
    {
        if (player1.bomb_num > 200)
        {
            player1.bomb_num = 200;
        }
        else
        {
            player1.bomb[player1.bomb_num].x = player1.x;
            player1.bomb[player1.bomb_num].y = player1.y;
            player1.bomb[player1.bomb_num].state = 1;
        }
        player1.bomb_num++;
    }
    for (int i = 0; i < player1.bomb_num; i++)
    {
        int collision_bomb[4] = {-1, -1, -1, -1};
        if (player1.bomb[i].state == 1)
        {
            drawGameAsset(player1.bomb[i].frame, player1.bomb[i].x, player1.bomb[i].y, bomb_width, bomb_height, bomb_allArray);
            if (player1.bomb[i].frame > 4)
            {
                for (int j = 0; j < player1.bomb_range+1; j++)
                { // 4 here is max range change this later
                    int bomb_directions[4][2] = {{player1.bomb[i].x, player1.bomb[i].y - 46 * j},
                                                 {player1.bomb[i].x, player1.bomb[i].y + 46 * j},
                                                 {player1.bomb[i].x - 38 * j, player1.bomb[i].y},
                                                 {player1.bomb[i].x + 38 * j, player1.bomb[i].y}};

                    for (int k = 0; k < 4; k++)
                    {
                        int collision_type = collision_detection(map,characters, bomb_directions[k][0], bomb_directions[k][1]);
                        if (collision_type == -2 && collision_bomb[k] == -1)
                        {   
                            drawGameAsset(player1.bomb[i].frame - 4, bomb_directions[k][0], bomb_directions[k][1], explosion_width, explosion_height, bomb_explosion_allArray);
                        }else if(collision_type >=0 && collision_bomb[k] == -1){
                            drawGameAsset(player1.bomb[i].frame - 4, bomb_directions[k][0], bomb_directions[k][1], explosion_width, explosion_height, bomb_explosion_allArray);
                            *hit_player = collision_type;
                        }
                        else
                        {
                            collision_bomb[k] = k;
                            
                        }
                    }
                }
            }
        }
    }
    return player1;
}

human character1_init(int x, int y, int moveup_offset, int is_npc,unsigned int frame_max, unsigned int frame_width, unsigned int frame_height, int health)
{
    human character1;
    character1.bomb_range = 2;
    character1.bomb_damage = 1;
    character1.mode = 0;
    character1.frame = 0;
    character1.is_alive = 1;
    character1.frame_width = frame_width;
    character1.frame_height = frame_height;
    character1.frame_max = frame_max;
    character1.is_npc = is_npc;
    character1.prior_x = x;
    character1.prior_y = y;
    character1.x = x;
    character1.y = y;
    character1.moveup_frame_offset = 0;
    character1.movedown_frame_offset = moveup_offset*2;
    character1.moveleft_frame_offset = moveup_offset*3;
    character1.moveright_frame_offset = moveup_offset*1;
    character1.offset = 8;
    character1.move_index = 0;
    character1.bomb_num = 0;
    character1.health = health;
    character1.damage = 5;
    character1.range = 2;
    return character1;
}


human controlCharater(int map[][28], human characters[], human player1, char c, int is_collision, int *hit_player, const unsigned long *frame_array[])
{   
    if(player1.is_npc){
        drawRectARGB32(player1.x, player1.y, player1.x + player1.frame_width, player1.y + player1.frame_height, 0x00000000, 1);
    }
    if(player1.is_alive == 0){
        player1.x =500;
        player1.y =0;
        return player1;
    }
    if (is_collision)
    {
        
        return player1;
    }
    
    

    if (c == 'd' || c == 'a' || c == 'w' || c == 's' || c == 'p')
    {   


        player1.frame++;
        if (player1.frame > player1.frame_max)
        {
            player1.frame = 0;
        }
        if(!player1.is_npc){
            drawRectARGB32(player1.x, player1.y, player1.x + player1.frame_width, player1.y + player1.frame_height, 0x00000000, 1);
        }
        player1.prior_x = player1.x;
        player1.prior_y = player1.y;
    }
    if (c == 'd')
    {
        player1.offset = player1.moveright_frame_offset + player1.frame;
        player1.x += 5;
    }
    else if (c == 'a')
    {
        player1.offset = player1.moveleft_frame_offset + player1.frame;
        player1.x -= 5;
    }
    else if (c == 'w')
    {
        player1.offset = player1.moveup_frame_offset + player1.frame;
        player1.y -= 5;
    }
    else if (c == 's')
    {
        player1.offset = player1.movedown_frame_offset + player1.frame;
        player1.y += 5;
    }
    else if (c == 'j' || player1.bomb_num)
    {
        player1 = plant_bomb(map,characters, player1, c, hit_player);
    }
    if(c == 'h' && player1.is_npc){
        player1.offset = player1.moveleft_frame_offset + player1.frame_max + 1;
    }

    drawGameAsset(player1.offset, player1.x, player1.y, player1.frame_width, player1.frame_height, frame_array);

    return player1;
}


human move(int map[][28], human players[], human npc, moves moves[], unsigned int move_size, int *hit_player, const unsigned long *frame_array[], int is_loop)
{
    //human temp = controlCharater(players, npc, moves[npc.move_index].direction, is_collision, hit_player, frame_array);
    human temp = controlCharater(map,players, npc, moves[npc.move_index].direction, tracking_player_on_map(npc, map, moves[npc.move_index].direction), hit_player, frame_array);
    

    if(moves[temp.move_index].direction == 'w'){
        if(npc.x/38 == moves[temp.move_index].distance[0] && (npc.y+36)/46 == moves[temp.move_index].distance[1]){
            temp.move_index = temp.move_index+1;
        }
    }else if(moves[temp.move_index].direction == 'a'){
        if((npc.x+28)/38 == moves[temp.move_index].distance[0] && npc.y/46 == moves[temp.move_index].distance[1]){
            temp.move_index = temp.move_index+1;
        }
    }
    else{
        if((npc.x+5)/38 == moves[temp.move_index].distance[0] && npc.y/46 == moves[temp.move_index].distance[1]){
            temp.move_index = temp.move_index+1;
        }
    }

    if (temp.move_index > move_size - 1)
    {
        temp.move_index = 0;
    }
   
    return temp;
}


