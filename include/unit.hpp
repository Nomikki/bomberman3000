#ifndef UNIT_HPP_INCLUDED
#define UNIT_HPP_INCLUDED

#include "common.hpp"

/**
 * @brief 
 * 
 */
enum ABILITYS
{
    ABI_NONE,
    ABI_SPEED,
    ABI_TIMER,
    ABI_COOLDOWN,
    ABI_MOVER,
    ABI_RANGE,
    ABI_MAX
};

enum Directions
{
    DOWN = 0,
    LEFT = 1,
    RIGHT = 2,
    UP = 3
};

class Unit
{

    int x, y;
    float lastX, lastY;
    float dx, dy;
    bool canMove;
    float movingCounter;
    bool alive;
    int direction;
    int animFrame;
    float animSpeed;
    float animCounter;
    bool animPlay;
    int ominaisuus;
    int howManyAnimFramesLeft;
    bool bombDropped;

public:
    float bombCooldown;

    Unit();
    ~Unit();

    /**
     * @brief Set unit to position.
     * But only if it can move.
     * 
     * @param _x 
     * @param _y 
     */
    void setPos(float _x, float _y);

    /**
     * @brief Set unit to position without checking if it can move.
     * 
     * @param px 
     * @param py 
     */
    void setPosForce(float px, float py);

    /**
     * @brief Set unit to position without checking if it can move.
     * 
     * @param _x 
     * @param _y 
     */
    void setPosForce2(int _x, int _y);

    /**
     * @brief Set the facing direction.
     * 
     * @param _dir 
     */
    void setDirection(int _dir);

    /**
     * @brief Get the current diretion of unit.
     * 
     * @return int 
     */
    int getDirection();

    /**
     * @brief get X
     * 
     */
    int getX();

    /**
     * @brief get Y
     * 
     */
    int getY();

    /**
     * @brief get delta X
     * 
     */
    float getdX();

    /**
     * @brief get delta Y
     * 
     */
    float getdY();

    /**
     * @brief Do player unit's basic logic. WIP.
     * Not the best function and need more from main.cpp.
     * Check if its alive or not.
     * Move it to right position
     * Handle animation counters
     * 
     * @param dt time between frames
     */
    void doLogic(float dt);

    /**
     * @brief Set the unit alive or kill it.
     * 
     * @param v if false, reset animation.
     */
    void setAlive(bool v);

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool isAlive();

    /**
     * @brief Set ability to unit.
     * 
     * @param v Only one ability can running.
     */
    void setAbility(int v);

    /**
     * @brief Get the current ability.
     * 
     * @return int 
     */
    int getAbility();

    /**
     * @brief Get the animation frame
     * 
     * @return int 
     */
    int getAnimFrame();

    /**
     * @brief Play/Stop current animation
     * 
     * @param v 
     */
    void playAnimation(bool v);

    /**
     * @brief Force to specific frame.
     * 
     * @param v 
     */
    void forceAnim(int v);

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool isBombDropped();

    /**
     * @brief Set the Bomb Dropped object
     * 
     * @param v 
     */
    void setBombDropped(bool v);
};

#endif // UNIT_HPP_INCLUDED
