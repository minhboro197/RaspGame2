#include "character.h"
#include "map.h"
#include "wall_block.h"
#include "../uart/uart1.h"

void draw_map_from_array(int map[][28])
{
    for (int i = 0; i < 18; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            if (map[i][j] > 0)
            {
                drawGameAsset(map[i][j] - 1, j * block_width, i * block_height, block_width, block_height, wall_block_allArray);
            }
        }
    }
}

void draw_stats() {
    
}