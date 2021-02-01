#include "level.hpp"
#include "sounds.hpp"

Level lvl;

int itemCounter = 0;

void clearLos()
{
    for (int y = 0; y < MAPY; y++)
    {
        for (int x = 0; x < MAPX; x++)
        {
            lvl.los[x][y] = -1;
        }
    }
}

void calcLos(float px, float py, float dt)
{
    for (int y = 0; y < MAPY; y++)
    {
        for (int x = 0; x < MAPX; x++)
        {
            if (lvl.los[x][y] >= 64.0)
                lvl.los[x][y] -= 200.0 * dt;
            else
                lvl.los[x][y] = -1.0;
        }
    }

    int len = 20;

    float x = px;
    float y = py;

    for (int a = 0; a < 360; a++)
    {
        float dx = sin(a / 180.0 * 3.14);
        float dy = cos(a / 180.0 * 3.14);
        x = px;
        y = py;

        for (int l = 0; l < len; l++)
        {
            lvl.los[(int)x][(int)y] = (len - l) * 20.0;

            if (lvl.los[(int)x][(int)y] > 255.0)
                lvl.los[(int)x][(int)y] = 255.0;

            if (x < 0 || y < 0 || x >= MAPX - 1 || y >= MAPY - 1)
            {
                break;
            }

            if (isCollide((int)x, (int)y))
            {
                lvl.los[(int)x][(int)y] = 255;
                break;
            }

            x += dx;
            y += dy;
        }
    }
}

int searchItem(int x, int y)
{
    for (unsigned int i = 0; i < lvl.items.size(); i++)
    {
        if (lvl.items[i].x == x && lvl.items[i].y == y)
        {
            return i;
        }
    }
    return -1;
}

bool isCollide(int x, int y)
{
    if (x < 0 || y < 0 || x > MAPX - 1 || y >= MAPY - 1)
    {
        return true;
    }

    for (unsigned int i = 0; i < lvl.items.size(); i++)
    {
        if (lvl.items[i].id == 0 || lvl.items[i].id == 10 || lvl.items[i].id == 20 || lvl.items[i].id == 30)
        {
            if (lvl.items[i].x == x && lvl.items[i].y == y)
            {
                return true;
            }
        }
    }

    if (lvl.id[x][y] == 1)
    {
        return false;
    }

    return true;
}

void addItem(int x, int y, int itemType, int ownerID)
{
    Item it;
    it.id = itemType;
    it.lifetime = 1.0;
    if (itemType == 10)
        it.lifetime = 3;
    if (itemType == 20)
        it.lifetime = 5.0;
    if (itemType == 30)
        it.lifetime = 3.0;

    addToSoundPool(SOUND_ID_death1, x, y, 1.0);

    it.ddx = x;
    it.ddy = y;
    it.dx = 0;
    it.dy = 0;
    it.x = x;
    it.y = y;
    it.animFrame = 0;
    it.animSpeed = 0.125;
    it.animCounter = 0.0;
    it.ownerID = ownerID;

    itemCounter++;
    it.itemID = itemCounter;

    lvl.items.push_back(it);
}

void initLevel(int seed)
{
    //printf("init map, seed num: %i\n", seed);
    srand(seed);

    lvl.items.clear();
    lvl.explosives.clear();

    for (int y = 0; y < MAPY; y++)
    {
        for (int x = 0; x < MAPX; x++)
        {
            if (y == 0 || x == 0 || y == MAPY - 1 || x == MAPX - 1)
            {
                lvl.id[x][y] = 0;
            }
            else
            {
                lvl.id[x][y] = 1;

                if (rand() % 10 > 4)
                {
                    if (x < 4 && y < 4)
                    {
                    }
                    else if (x > MAPX - 5 && y < 4)
                    {
                    }
                    else if (x < 4 && y > MAPY - 5)
                    {
                    }
                    else if (x > MAPX - 5 && y > MAPY - 5)
                    {
                    }
                    else
                    {
                        lvl.id[x][y] = 2;
                    }
                }

                if (x % 2 == 0)
                {
                    if (y % 2 == 0)
                    {
                        lvl.id[x][y] = 0;
                    }
                }
            }
        }
    }

    std::vector<int> availItems;
    availItems.clear();

    if (lvl.rules.itemSpeed)
        availItems.push_back(1);
    if (lvl.rules.itemTimer)
        availItems.push_back(2);
    if (lvl.rules.itemCooldown)
        availItems.push_back(3);
    if (lvl.rules.itemMover)
        availItems.push_back(4);
    if (lvl.rules.itemRange)
        availItems.push_back(5);

    if (availItems.size() > 0)
    {
        for (int i = 0; i < 50; i++)
        {
            int x, y, id;
            x = 1 + rand() % (MAPX - 2);
            y = 1 + rand() % (MAPY - 2);

            if (availItems.size() > 1)
                id = availItems[(rand() % (availItems.size()))];
            else
                id = availItems[0];

            if (lvl.id[x][y] == 2)
            {
                bool fail = false;
                for (unsigned int k = 0; k < lvl.items.size(); k++)
                {
                    if (lvl.items[k].x == x && lvl.items[k].y == y)
                    {
                        fail = true;
                        break;
                    }
                }
                if (!fail)
                {
                    addItem(x, y, id, -1);
                }
            }
        }
    }
}

Level *getLvl()
{
    return &lvl;
}
