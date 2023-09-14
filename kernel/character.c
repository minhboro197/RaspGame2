
#include "character.h"
#include "bomb.h"
#include "magewalking.h"
#include "framebf.h"
#include "bomb_explosion.h"
#include "map_array.h"
#include "../uart/uart1.h"
// Track player on the map and collison dection with walls
int tracking_player_on_map(human player, int map[][28], char c)
{
    int player_width = 25;
    int player_height = 41;
    int clearance = 5;

    if (c == 'a')
    {
        if (map[(player.y + player_height) / 46][(player.x - (clearance+3)) / 38] > 0 || map[(player.y) / 46][(player.x - (clearance+3)) / 38] > 0)
        {
            return 1;
        }
    }
    else if (c == 'd')
    {
        if (map[(player.y + player_height) / 46][(player.x + player_width + (clearance+3)) / 38] > 0 || (map[(player.y) / 46][(player.x + player_width + (clearance+3)) / 38] > 0))
        {
            return 1;
        }
    }
    else if (c == 'w')
    {
        if (map[(player.y - clearance) / 46][(player.x + player_width + clearance) / 38] > 0 || map[(player.y - clearance) / 46][(player.x) / 38] > 0)
        {
            return 1;
        }
    }
    else if (c == 's')
    {
        if (map[(player.y + player_height + clearance) / 46][(player.x + player_width) / 38] > 0 || map[(player.y + player_height + clearance) / 46][(player.x) / 38] > 0)
        {
            return 1;
        }
    }

    return 0;
}

int collision_detection(human humans[], unsigned int object_x, unsigned int object_y){

    for(int i = 0; i < 3; i++){
        if(absolute(object_x - humans[i].x) < 28 && absolute(object_y - humans[i].y) < 29){
            uart_dec(absolute(object_y - humans[i].y));
            humans[i].health -= 5;
            return 1;
        }
    }

    int bounding_box[][2] = {{object_y, object_x},
                            {object_y, object_x + 25},
                            {object_y + 25, object_x},
                            {object_y + 25, object_x +25}};
    for(int i =0; i < 4; i++){
        if(map2[bounding_box[i][0]/46][bounding_box[i][1]/38] > 0){
            return 2;
        }
    }
    return 0;
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

int ignore_collision_after_explosion = 0;
human plant_bomb(human characters[], human player1, char c)
{
    if (c == 'j')
    {
        if (player1.bomb_num > 4)
        {
            player1.bomb_num = 4;
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
                for (int j = 0; j < player1.range+1; j++)
                { // 4 here is max range change this later
                    int bomb_directions[4][2] = {{player1.bomb[i].x, player1.bomb[i].y - 46 * j},
                                                 {player1.bomb[i].x, player1.bomb[i].y + 46 * j},
                                                 {player1.bomb[i].x - 38 * j, player1.bomb[i].y},
                                                 {player1.bomb[i].x + 38 * j, player1.bomb[i].y}};

                    for (int k = 0; k < 4; k++)
                    {
                        int collision_type = collision_detection(characters, bomb_directions[k][0], bomb_directions[k][1]);
                        if (collision_type == 0 && collision_bomb[k] == -1)
                        {
                            drawGameAsset(player1.bomb[i].frame - 4, bomb_directions[k][0], bomb_directions[k][1], explosion_width, explosion_height, bomb_explosion_allArray);
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

human character1_init(int x, int y, int moveup_offset)
{
    human character1;
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
    character1.health = 4;
    character1.damage = 5;
    character1.range = 2;
    return character1;
}

unsigned int absolute(int num)
{
    if (num < 0)
    {
        num = num * -1;
        return num;
    }
    return num;
}

int frame = 0;
human controlCharater(human characters[], human player1, char c, int is_npc, int is_collision, int frame_max)
{
    if (is_collision)
    {
        player1.x = player1.prior_x;
        player1.y = player1.prior_y;
        return player1;
    }
    if (!is_npc)
    {
        player1.prior_x = player1.x;
        player1.prior_y = player1.y;
    }

    if (c == 'd' || c == 'a' || c == 'w' || c == 's')
    {
        frame++;
        if (frame > frame_max)
        {
            frame = 0;
        }
        drawRectARGB32(player1.x, player1.y, player1.x + mage_width, player1.y + mage_height, 0x00000000, 1);
    }
    if (c == 'd')
    {
        player1.offset = player1.moveright_frame_offset + frame;
        player1.x += 5;
    }
    else if (c == 'a')
    {
        player1.offset = player1.moveleft_frame_offset + frame;
        player1.x -= 5;
    }
    else if (c == 'w')
    {
        player1.offset = player1.moveup_frame_offset + frame;
        player1.y -= 5;
    }
    else if (c == 's')
    {
        player1.offset = player1.movedown_frame_offset + frame;
        player1.y += 5;
    }
    else if (c == 'j' || player1.bomb_num)
    {
        player1 = plant_bomb(characters, player1, c);
    }

    drawGameAsset(player1.offset, player1.x, player1.y, mage_width, mage_height, mage_walking_allArray);

    return player1;
}

human move(human players[], human npc, moves moves[], unsigned int move_size, int is_collision, int max_frame)
{
    human temp = controlCharater(players, npc, moves[npc.move_index].direction, 1, is_collision,max_frame);

    if (absolute(temp.x - temp.prior_x) == moves[temp.move_index].distance)
    {
        temp.prior_x = temp.x;
        temp.move_index = temp.move_index + 1;
    }
    else if (absolute(temp.y - temp.prior_y) == moves[temp.move_index].distance)
    {
        temp.prior_y = temp.y;
        temp.move_index = temp.move_index + 1;
    }

    if (temp.move_index > move_size - 1)
    {
        temp.move_index = 0;
    }
    return temp;
}
