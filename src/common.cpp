#include "common.hpp"
#include "chat.hpp"

std::vector<Player> namelist;
int seedNum = 0;
sf::Mutex commonLock;

//split function that returns array of strings
std::vector<std::string> explode(std::string s, std::string e)
{
    std::vector<std::string> ret;
    int iPos = (int)s.find(e, 0);
    int iPit = (int)e.length();

    while (iPos > -1)
    {
        if (iPos != 0)
            ret.push_back(s.substr(0, iPos));

        s.erase(0, iPos + iPit);  
        iPos = (int)s.find(e, 0);
    }

    if (s != "")
        ret.push_back(s);

    return ret;
}

void addToNames(std::string name)
{
    Player pl;
    pl.connected = true;
    pl.name = name;
    pl.ready = false;

    for (unsigned int i = 0; i < namelist.size(); i++)
    {
        if (name == namelist[i].name)
        {
            remFromNames(name);
            namelist.push_back(pl);
            return;
        }
    }

    namelist.push_back(pl);
}

void remFromNames(std::string name)
{
    for (unsigned int i = 0; i < namelist.size(); i++)
    {
        if (name == namelist[i].name)
        {
            namelist.erase(namelist.begin() + i);
            return;
        }
    }
}

void parseNetPacket(const char *data)
{
    
    std::vector<std::string> splittedBuf = explode(data, "|");

    if (splittedBuf.size() > 0)
    {
        if (splittedBuf[0] == "MSG")
        {
            addChatText((char *)splittedBuf[1].c_str(), (char *)splittedBuf[2].c_str());   
        }

        /*
        if (splittedBuf[0] == "YOUR_ID") {
        }
        */

        if (splittedBuf[0] == "HI")
        {
            addChatText((char *)splittedBuf[1].c_str(), (char *)"joined to lobby");
            addToNames(splittedBuf[1]);
        }

        if (splittedBuf[0] == "NAME")
        {
            int re = atoi((char *)splittedBuf[2].c_str());

            addToNames(splittedBuf[1]);

            for (unsigned int a = 0; a < namelist.size(); a++)
            {
                if (splittedBuf[1] == namelist[a].name)
                {
                    if (re == 1)
                        namelist[a].ready = 1;
                    else
                        namelist[a].ready = 0;
                    break;
                }
            }
        }

        if (splittedBuf[0] == "DC")
        {
            //printf("Koita poistaa nimi '%s'\n", (char*)splittedBuf[1].c_str());
            remFromNames(splittedBuf[1]);
        }
        if (splittedBuf[0] == "READY")
        {
            for (unsigned int i = 0; i < namelist.size(); i++)
            {
                if (namelist[i].name == splittedBuf[1])
                {
                    if (atoi((char *)splittedBuf[2].c_str()) == 1)
                        namelist[i].ready = true;
                    else
                        namelist[i].ready = false;
                }
            }
        }

        if (splittedBuf[0] == "BOMB")
        {
            destroyMyBomb(atoi((char *)splittedBuf[1].c_str()));
        }

        if (splittedBuf[0] == "UNREADY")
        {
            setGameStatus(G_MAINMENU);
        }

        if (splittedBuf[0] == "POS")
        {
            int id, x, y, dir;
            id = atoi((char *)splittedBuf[1].c_str());
            x = atoi((char *)splittedBuf[2].c_str());
            y = atoi((char *)splittedBuf[3].c_str());
            dir = atoi((char *)splittedBuf[4].c_str());

            if (id != getMYID())
            {
                setUnitPos(id, x, y, dir);
            }
        }

        if (splittedBuf[0] == "CLEAR_NAMES")
        {
            namelist.clear();
            sendHi();
        }

        if (splittedBuf[0] == "ADDITEM")
        {
            int itemType = atoi((char *)splittedBuf[1].c_str());
            int x = atoi((char *)splittedBuf[2].c_str());
            int y = atoi((char *)splittedBuf[3].c_str());
            int ownID = atoi((char *)splittedBuf[4].c_str());

            addItem(x, y, itemType, ownID);
        }

        if (splittedBuf[0] == "RULES")
        {
            setRules(atoi((char *)splittedBuf[1].c_str()),
                     atoi((char *)splittedBuf[2].c_str()),
                     atoi((char *)splittedBuf[3].c_str()),
                     atoi((char *)splittedBuf[4].c_str()),
                     atoi((char *)splittedBuf[5].c_str()),
                     atoi((char *)splittedBuf[6].c_str()));
        }
        if (splittedBuf[0] == "DEAD")
        {
            setUnitToDead(atoi((char *)splittedBuf[1].c_str()));
        }

        if (splittedBuf[0] == "ITEMDIR")
        {
            changeItemDirection(
                atoi((char *)splittedBuf[1].c_str()),
                atoi((char *)splittedBuf[2].c_str()),
                atoi((char *)splittedBuf[3].c_str()));
        }
    }
    
}

std::vector<Player> *getNamelist()
{
    return &namelist;
}

void setSeed(unsigned int num)
{
    seedNum = num;
}

unsigned int getSeed()
{
    return seedNum;
}

float distance(float sx, float sy, float ex, float ey)
{
    float dx = (ex - (sx));
    float dy = (ey - (sy));
    dx *= dx;
    dy *= dy;
    return sqrtf(dx + dy);
}
