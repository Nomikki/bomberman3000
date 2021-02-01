#include "Unit.hpp"
#include "sounds.hpp"

Unit::Unit()
{
    x = 0;
    y = 0;
    dx = 0;
    dy = 0;
    bombCooldown = 0.0;

    animSpeed = 0.15;
    animCounter = 0;
    animFrame = 0;
    howManyAnimFramesLeft = 0;
}

Unit::~Unit()
{
}

void Unit::setPos(float _x, float _y)
{
    if (canMove)
    {
        x = _x;
        y = _y;
        canMove = false;

        movingCounter = 0.1;

        if (getAbility() == ABI_SPEED)
            movingCounter = 0.1 * 0.75;
    }
}

void Unit::setDirection(int _dir)
{
    direction = _dir;
}

int Unit::getDirection()
{
    return direction;
}

int Unit::getX()
{
    return x;
}

int Unit::getY()
{
    return y;
}

float Unit::getdX()
{
    return dx;
}

float Unit::getdY()
{
    return dy;
}

void Unit::setPosForce(float px, float py)
{
    x = px;
    y = py;
    dx = x;
    dy = y;
}

void Unit::setPosForce2(int _x, int _y)
{
    lastX = x;
    lastY = y;
    x = _x;
    y = _y;
}

void Unit::setAlive(bool v)
{
    if (!v)
    {
        animFrame = 0;
    }
    bombDropped = false;
    alive = v;
}
bool Unit::isAlive()
{
    return alive;
}

void Unit::doLogic(float dt)
{
    if (isAlive())
    {
        if (x == int(dx + 0.5) && y == int(dy + 0.5))
        {
            canMove = true;
        }
    }
    else
    {
        canMove = false;
    }

    bombCooldown -= dt;
    if (bombCooldown <= 0.0)
        bombCooldown = 0.0;

    if (getAbility() == ABI_SPEED)
    {
        animSpeed = 0.15 * 0.75;
    }
    else
    {
        animSpeed = 0.15;
    }

    if (animPlay || howManyAnimFramesLeft > 0)
    {
        animCounter += dt;
        if (animCounter >= animSpeed)
        {
            animCounter -= animSpeed;
            animFrame++;

            if (isAlive())
                addToSoundPool(SOUND_ID_walk01 + (rand() % 4), x, y, 1.0);

            if (howManyAnimFramesLeft > 0)
                howManyAnimFramesLeft--;
            else
                howManyAnimFramesLeft = 0;

            if (!isAlive())
            {
                if (animFrame > 3)
                    animFrame = 3;
            }
            else
            {
                if (animFrame > 3)
                    animFrame = 0;
            }
        }
    }

    float nop = 4.0;

    if (getAbility() == ABI_SPEED)
    {
        nop = 6.0;
    }

    for (int k = 0; k < 10; k++)
    {
        float la = nop * dt * 0.1;
        if (dx < x)
            dx += la;
        if (dx > x)
            dx -= la;
        if (dy < y)
            dy += la;
        if (dy > y)
            dy -= la;
    }

    float eX = (dx - x) / 1.0 * dt;
    float eY = (dy - y) / 1.0 * dt;

    dx -= eX;
    dy -= eY;

    lastX = dx;
    lastY = dy;
}

void Unit::forceAnim(int v)
{
    howManyAnimFramesLeft = v;
}

void Unit::playAnimation(bool v)
{
    animPlay = v;
}

int Unit::getAnimFrame()
{
    return animFrame;
}

void Unit::setAbility(int v)
{
    ominaisuus = v;
}

int Unit::getAbility()
{
    return ominaisuus;
}

bool Unit::isBombDropped()
{
    return bombDropped;
}

void Unit::setBombDropped(bool v)
{
    bombDropped = v;
}
