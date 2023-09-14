#include "../uart/uart1.h"
#include "character.h"
#include "framebf.h"
#include "map.h"
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

void npc_plant_bomb(human npc, human player)
{
    if (absolute(npc.x - player.x) < 50 && absolute(npc.y - player.y) < 50)
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
void play_game()
{
    int step = 38 * 3;
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

    // wall block width and height is 38 and 46
    // span npc in the below formar
    human player1 = character1_init(38, 46 * 3, 9);
    human npc = character1_init(38 * 10, 46 * 10, 9);
    human npc2 = character1_init(38 * 5, 46 * 9, 9);
    // 1 seconds = 1000000
    set_wait_timer(1, 10000); // set 10ms
    int bomb_delay = 0;
    int once = 0;

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

    while (1)
    {
        if (once == 0)
        {
            once = 1;
            draw_map_from_array(map2);
        }

        human characters[] = {player1, npc, npc2};
        char c = getUart();
        player1 = controlCharater(characters, player1, c, 0, tracking_player_on_map(player1, map2, c),8);
        int timer = set_wait_timer(0, 0);
        // uart_dec(player1.bomb_num);
        // uart_sendc('\n');

        if (timer)
        {
            if ((timer % 10) == 0)
            { // every 100ms

                npc = move(characters, npc, npc1_moves, sizeof(npc1_moves) / sizeof(npc1_moves[0]), 0,8);
                npc = controlCharater(characters, npc, 't', 1, 0,8); // refresh character
                npc2 = move(characters, npc2, npc2_moves, sizeof(npc2_moves) / sizeof(npc2_moves[0]), 0,8);
                npc2 = controlCharater(characters, npc2, 't', 1, 0,8); // refresh character

                // npc_plant_bomb(npc,player1);
                // npc_plant_bomb(npc2,player1);

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

    // echo everything back
    play_game();
}