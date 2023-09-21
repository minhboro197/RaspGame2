
/*

int flood_fill(int map[][28], int x, int y, int current){
    if(y > 16 || x > 27){
        return current;
    }
    if(map[y][x] == -1 || map[y][x] >= 0){
        return 0;
    }
    if(map[y][x]== -2){
        map[y][x] = current;
        current++;
    }

    flood_fill(map, x-1,y,current);
    flood_fill(map, x,y+1,current);
    flood_fill(map, x+1,y,current);
    flood_fill(map, x,y-1,current);

    return 0;
}


void plan_routes(human npc, moves *npc_chase){
    int cor_x = npc.x/block_width;
    int cor_y = npc.y/block_height;

    int index = 0;
    int min_index = 0;
    char awds = ' ';
    uart_puts("tf\n");
    
    while(1){
        if(flood_map[cor_y][cor_x] == 1){
            break;
        }
        int min = 1000;
        int direction[] = {-1,-1,-1,-1};
        direction[0] = flood_map[cor_y-1][cor_x];
        direction[1]= flood_map[cor_y+1][cor_x];
        direction[2] = flood_map[cor_y][cor_x-1];
        direction[3] = flood_map[cor_y][cor_x+1];

        for(int i = 0; i < 4; i++){
            if(direction[i] < min && direction[i] >=0){
                uart_dec(direction[i]);
                uart_sendc('\n');
                min = direction[i];
                min_index = i;
            }
        }
        
        if(min_index == 0){
            cor_y -= 1;
            npc_chase[index].direction = 'w';
        }else if(min_index == 1){
            cor_y += 1;
            npc_chase[index].direction = 's';
        }else if(min_index == 2){
            cor_x -= 1;
            npc_chase[index].direction = 'a';
        }else if(min_index == 3){
            cor_x += 1;
            npc_chase[index].direction = 'd';
        }
        npc_chase[index].distance[0] = cor_x;
        npc_chase[index].distance[1] = cor_y;
        
        index++;
        
    }
    
}
*/