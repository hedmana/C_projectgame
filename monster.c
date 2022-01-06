/* monster.c -- Implementation of monster actions
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "dungeon.h"

// for defining some monster types below that can be used in the game
typedef struct {
    char name[20];  // Name of monster
    char sign;  // character to show it on map
    unsigned int hplow;  // lowest possible initial maxhp
    unsigned int hphigh;  // highest possible initial maxhp
} MonstType;

// Specifying three monster types to start with.
// Feel free to add more, or change the below
// Note that it is up to you to decide whether to use this array from createMonsters
// you may or may not use it
const MonstType types[] = {
    { "Goblin", 'G', 6, 10},
    { "Rat", 'R', 3, 5},
    { "Dragon", 'D', 15, 20}
};


/* One kind of attack done by monster.
 * The attack function pointer can refer to this.
 * 
 * Parameters:
 * game: the game state
 * monst: The monster performing attack
 */
void attackPunch(Game *game, Creature *monst) {
    printf("%s punches you! ", monst->name);
    int hitprob = 50;
    int maxdam = 4;
    if (rand() % 100 < hitprob) {
        printf("Hit! ");
        int dam = rand() % maxdam + 1;
        printf("Damage: %d ", dam);
        game->hp = game->hp - dam;
        if (game->hp <= 0)
            printf("You died!");
        printf("\n");
    } else {
        printf("Miss!\n");
    }
}



/* Exercise (c)
 *
 * Move monster 'monst' towards the player character.
 * See exercise description for more detailed rules.
 */
int isCloserToPlayer(const Game *game, int x_current, int y_current, int x_pp, int y_pp)
{
    // vi vill jämföra två avstånd
    
    return (abs(game->position.x - x_pp) + abs(game->position.y - y_pp) < abs(game->position.x - x_current) + abs(game->position.y - y_current))? 1 : 0;
}
// kollar om möjlig position är densamma som spelarens position
// returnerar 1 ifall det inte är och 0 om spelaren finns där
int isOnTopOfPlayer(const Game *game, int x, int y)
{
    return(game->position.x == x && game->position.y == y) ? 1 : 0;
}

void moveTowards(const Game *game, Creature *monst) {

    int change_pos[4][2] = {{0,1}, {0,-1}, {1,0}, {-1,0}};
    
    for(int i = 0; i<4; i++)
    {
        int x = monst->pos.x + change_pos[i][0];
        int y = monst->pos.y + change_pos[i][1];
        
        if(!isBlocked(game,x,y) && isCloserToPlayer(game, monst->pos.x, monst->pos.y, x,y) && !isOnTopOfPlayer(game, x, y))
        {
            monst->pos.x = x;
            monst->pos.y = y;
        }
    }
    
}

/* Exercise (d)
 *
 * Move monster 'monst' away from the player character.
 * See exercise description for more detailed rules.
 */
void moveAway(const Game *game, Creature *monst) {

	int change_pos[4][2] = {{0,1}, {0,-1}, {1,0}, {-1,0}};
    
    for(int i = 0; i<4; i++)
    {
        int x = monst->pos.x + change_pos[i][0];
        int y = monst->pos.y + change_pos[i][1];
        
        if(!isBlocked(game,x,y) && !isCloserToPlayer(game, monst->pos.x, monst->pos.y, x,y) && !isOnTopOfPlayer(game, x, y))
        {
            monst->pos.x = x;
            monst->pos.y = y;
        }
    }
    

    
}



/* Exercise (e)
 *
 * Take action on each monster (that is alive) in 'monsters' array.
 * Each monster either attacks or moves (or does nothing if no action is specified)
 */
void monsterAction(Game *game) {
	(void) game;

    for(unsigned int i = 0; i < (game->numMonsters);i++){

        if(game->monsters[i].hp > 0){

            if(abs(game->monsters[i].pos.x - game->position.x) <2 && abs(game->monsters[i].pos.y - game->position.y) <2 ){

                game->monsters[i].attack(game, &game->monsters[i]);
            }
            else{
                game->monsters[i].move(game, &game->monsters[i]);
            }
        }
    }
    
}



/* Exercise (b)
 *
 * Create opts.numMonsters monsters and position them on valid position
 * in the the game map. The moster data (hitpoints, name, map sign) should be
 * set appropriately (see exercise instructions) 
 */
void createMonsters(Game *game) {
    (void) game;
    unsigned int howMany = game->opts.numMonsters;
    game->monsters = malloc(howMany * sizeof(Creature));
    
    for(unsigned int i= 0; i < howMany;i++){
        int x = rand() % game -> opts.mapWidth;
        int y = rand() % game -> opts.mapHeight;
        while (isBlocked(game,x,y)) {
        x = rand() % game -> opts.mapWidth;
        y = rand() % game -> opts.mapHeight;
    }
   

    int type = rand() %3;
    strcpy(game->monsters[i].name, types[type].name);
    game->monsters[i].sign   = types[type].sign;
    game->monsters[i].maxhp = types[type].hphigh;
    game->monsters[i].hp = types[type].hphigh;
    game->monsters[i].pos.x  =  x;
    game->monsters[i].pos.y = y;
    if( rand() % 2 == 0){
        game->monsters[i].move = moveTowards;
    }
    else{
        game->monsters[i].move = moveAway;
    }
    game->monsters[i].attack = attackPunch;
    game->numMonsters++;
    }
    
}

/* Determine whether monster moves towards or away from player character.
 */
void checkIntent(Game *game)
{
    for (unsigned int i = 0; i < game->numMonsters; i++) {
        Creature *m = &game->monsters[i];
        if (m->hp <= 2) {
            m->move = moveAway;
        } else {
            m->move = moveTowards;
        }
        if (m->hp < m->maxhp)
            m->hp = m->hp + 0.1;  // heals a bit every turn
    }
}