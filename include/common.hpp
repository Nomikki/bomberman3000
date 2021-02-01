#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include <cmath>
#include <cstdio>
#include <vector>
#include <string.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#define FAIL -1
#define SUCCESS 0

#define ALIGN_LEFT 0x0
#define ALIGN_RIGHT 0x1
#define ALIGN_CENTER 0x2
#define ALIGN_CENTER_ALL 0x3

#define MAPX 25
#define MAPY 25

#define tilex 64
#define tiley 64

#define ApplicationName "POMPERMANI"

struct Controls
{
    bool up;
    bool down;
    bool strafe_left;
    bool strafe_right;

    bool dropBom;

    bool lmouse;
    bool rmouse;

    float mouseX, mouseY;
    float RmouseX, RmouseY;

    int rotated;
    float lookDir;
};

struct Camera
{
    float dx, dy;
    int followEnt;
    float movingSpeed;
};

struct Statistics
{
    int numEnts;
    int numEntRendered;
    float fps;
    float deltatimer; //time between rendered frames
};

struct DRECT
{
    int x, y, w, h;
};

enum GSTATUS
{
    G_NONE = 0,
    G_MAINMENU,
    G_OPTIONS,
    G_INGAME,
    G_MAX
};

struct Item
{
    int ownerID;
    int x, y, id;
    float ddx, ddy;
    int dx, dy;
    float lifetime;
    int animFrame;
    float animSpeed;
    float animCounter;
    int itemID;
};

struct Explo
{
    int x, y;
    int tx, ty; //piece of explosion
    int animFrame;
    float animSpeed;
    float animCounter;

    float lifetime;
};

struct Rules
{
    int playersNum;
    bool los;
    bool itemSpeed;
    bool itemTimer;
    bool itemCooldown;
    bool itemMover;
    bool itemRange;
};

struct Player
{
    std::string name;
    bool ready;
    bool connected;
};


/**
 * @brief split string to array
 * 
 * @param s text string
 * @param e separator string
 * @return std::vector<std::string> return array of texts
 */
extern std::vector<std::string> explode(std::string s, std::string e);

/**
 * @brief Parsing all netpackets.
 * 
 * @param data Netmessage from server.
 */
extern void parseNetPacket(const char *data);

/**
 * @brief Get the Namelist
 * 
 * @return std::vector<Player>* 
 */
extern std::vector<Player> *getNamelist();

/**
 * @brief Add name to list
 * 
 * @param name 
 */
extern void addToNames(std::string name);

/**
 * @brief Remove name from list
 * 
 * @param name 
 */
extern void remFromNames(std::string name);


/**
 * @brief Set the Game Status
 * 
 * @param st 
 */
extern void setGameStatus(int st);

/**
 * @brief Set the position of unit
 * 
 * @param id which unit 
 * @param x position
 * @param y 
 * @param direction facing direction / where to look 
 */
extern void setUnitPos(int id, int x, int y, int direction);

/**
 * @brief Send rules to everyone. 
 * 
 */
extern void sendRules();

/**
 * @brief Set the random seed
 * 
 * @param num 
 */
extern void setSeed(unsigned int num);

/**
 * @brief Get the generated random seed.
 * 
 * @return unsigned int 
 */
extern unsigned int getSeed();

/**
 * @brief Add item to level (like bomb). 
 * 
 * @param x 
 * @param y 
 * @param itemType 
 * @param ownerID Who drop the bomb.
 */
extern void addItem(int x, int y, int itemType, int ownerID);

/**
 * @brief Set the Rules in lobby
 * 
 * @param los 
 * @param cooldown 
 * @param mover 
 * @param range 
 * @param speed 
 * @param timer 
 */
extern void setRules(bool los, bool cooldown, bool mover, bool range, bool speed, bool timer);

/**
 * @brief Server is handling all IDs, and after joining it will send ID to us. 
 * 
 * @return int Return stored ID.
 */
extern int getMYID();

/**
 * @brief After joining to lobby, send hi message to server.
 * Server send namelist back to client and send hi message to everyone else.
 * Other clients will add new name to list and write to chat.
 */
extern void sendHi();

/**
 * @brief Get the My Nick 
 * 
 * @return std::string 
 */
extern std::string getMyNick();

/**
 * @brief Distance between 2 points (s and e)
 * 
 * @param sx 
 * @param sy 
 * @param ex 
 * @param ey 
 * @return float return sqrt((e - s)^2)
 */
extern float distance(float sx, float sy, float ex, float ey);

/**
 * @brief Draw text to screen.
 * 
 * @param _text 
 * @param x position (horizontal)
 * @param y position (vertical)
 * @param r color (red)
 * @param g color (green)
 * @param b color (blue)
 * @param a color (alpha)
 * @param align (left, middle, right)
 * @param toID fontID (which font is used)
 * @param size font size in pixels
 */
extern void drawTextB(std::string _text, int x, int y, int r, int g, int b, int a, int align, int toID, float size);

/**
 * @brief Set the Unit To Dead 
 * 
 * @param id Which unit
 */
extern void setUnitToDead(int id);

/**
 * @brief Because of remote control bombs, we need this function
 * 
 * @param id ID of player
 */
extern void destroyMyBomb(int id);

/**
 * @brief WIP. Push bomb to direciton.
 * 
 * @param itemID 
 * @param lx 
 * @param ly 
 */
extern void changeItemDirection(int itemID, int lx, int ly);

#endif // COMMON_HPP_INCLUDED
