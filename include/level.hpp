#ifndef LEVEL_HPP_INCLUDED
#define LEVEL_HPP_INCLUDED

#include "common.hpp"


/**
 * @brief Level structure is pretty straightforward.
 * There is rules, tile types, fov and items/explosions.
 * 
 */
struct Level
{
    Rules rules;
    int id[MAPX][MAPY];
    float los[MAPX][MAPY];

    std::vector<Item> items;
    std::vector<Explo> explosives;
};

/**
 * @brief Clear Field of vision
 * 
 */
extern void clearLos();

/**
 * @brief Calculate field of vision, means what player unit can see.
 * 
 * @param px position of player unit
 * @param py 
 * @param dt delta timer
 */
extern void calcLos(float px, float py, float dt);

/**
 * @brief 
 * 
 * @param x 
 * @param y 
 * @return true something blocking in position
 * @return false there is no any blocking things in position.
 */
extern bool isCollide(int x, int y);

/**
 * @brief Add item to level, lime bombs
 * 
 * @param x 
 * @param y 
 * @param itemType what kind of bomb
 * @param ownerID who own that bomb?
 */
extern void addItem(int x, int y, int itemType, int ownerID);

/**
 * @brief Even level is a bit simple, there is bricks randomly and every client need right random seed to generate exactly same level.
 * 
 * @param seed Random seed
 */
extern void initLevel(int seed);

/**
 * @brief Get the Lvl object
 * 
 * @return Level* 
 */
extern Level *getLvl();

/**
 * @brief Search level from position and send back index of item.
 * 
 * @param x 
 * @param y 
 * @return int Return -1 if item not found.
 */
extern int searchItem(int x, int y);

#endif // LEVEL_HPP_INCLUDED
