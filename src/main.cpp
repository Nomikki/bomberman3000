#include "common.hpp"
#include "settings.hpp"
#include "level.hpp"
#include "Unit.hpp"
#include "network.hpp"
#include "resources.hpp"
#include "chat.hpp"
#include "sounds.hpp"

Controls controls;

/**
 * @brief Game settings
 * 
 */
Settings *sets = NULL;

float zoom = 1.0;
float secTimer = 1.0;
Statistics monitoring;

sf::RenderWindow App;
sf::RenderTexture mWindow;
sf::Sprite sp;
Camera cam;

bool showNames = false;

int gameStatus = 0;
int menuPos = 0;  //mainmenu
int menuPos2 = 0; //options
int menuPos3 = 0; //etsi lobby

bool writeText = false;
std::string chatText = "";

Network net;
Player players[MAX_PLAYERS];
Unit Units[MAX_PLAYERS];

unsigned int playerAmount = 0;
int winners = 0;
float readyCounter = 0;
float winCounter = 0.0;
bool winnerFound = false;

float gameCounter = 0;
int seconds = 0;

bool stopThreads = false;
bool stopNetThread = false;
sf::Mutex mutex;

void initScreen()
{
    !sets->fullscreen ? App.create(sf::VideoMode(sets->screenX, sets->screenY), ApplicationName) : App.create(sf::VideoMode(sets->screenX, sets->screenY), ApplicationName, sf::Style::Fullscreen);

    sets->vsync ? App.setVerticalSyncEnabled(true) : App.setVerticalSyncEnabled(false);

    mWindow.create(sets->screenX, sets->screenY);
}

int init()
{
    initScreen();

    loadFonts();
    loadImgs();

    App.setFramerateLimit(sets->framelimit);
    return 0;
}

void deinit()
{
    
}

void zoomTo(float v)
{
    zoom += v;

    if (zoom < 0.25)
        zoom = 0.25;
    if (zoom > 3.0)
        zoom = 3.0;

    sf::View vi = mWindow.getView();
    vi.setSize(sets->screenX, sets->screenY);
    vi.zoom(zoom);
    mWindow.setView(vi);
}

void setCameraPos(float x, float y, float movingSpeed)
{
    cam.dx = x;
    cam.dy = y;
    cam.movingSpeed = movingSpeed;
}

void setCameraFollow(int followID, float x, float y, float movingSpeed)
{
    cam.dx = ((x) - ((sets->screenX * 0.5) / tilex));
    cam.dy = ((y) - ((sets->screenY * 0.5) / tiley));

    cam.followEnt = followID;
    cam.movingSpeed = movingSpeed;
}

int menuInput(float dt)
{
    sf::Event event;

    while (App.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            return 0;

        if (!App.hasFocus())
            return SUCCESS;

        if (event.type == sf::Event::Resized)
        {
            sf::Vector2u si;
            si.x = event.size.width;
            si.y = event.size.height;

            sets->screenX = si.x;
            sets->screenY = si.y;

            initScreen();
            zoomTo(0);
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                return 0;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                addToSoundPool(SOUND_ID_hit2, 0, 0, 1.0);
                return 1;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                addToSoundPool(SOUND_ID_hit4, 0, 0, 1.0);
                menuPos--;
                if (menuPos < 0)
                    menuPos = 4;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                addToSoundPool(SOUND_ID_hit4, 0, 0, 1.0);
                menuPos++;
                if (menuPos > 4)
                    menuPos = 0;
            }
        }
    }

    sf::Vector2i pixelPos = sf::Mouse::getPosition(App);
    sf::Vector2f worldPos = mWindow.mapPixelToCoords(pixelPos);
    controls.mouseX = worldPos.x;
    controls.mouseY = worldPos.y;

    return 0;
}

int searchInput(float dt)
{
    sf::Event event;

    while (App.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            return 0;

        if (!App.hasFocus())
            return SUCCESS;

        if (event.type == sf::Event::Resized)
        {
            sf::Vector2u si;
            si.x = event.size.width;
            si.y = event.size.height;

            sets->screenX = si.x;
            sets->screenY = si.y;

            initScreen();
            zoomTo(0);
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                return 0;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
            {
                addToSoundPool(SOUND_ID_hit2, 0, 0, 1.0);

                if (menuPos3 != 4 && menuPos3 != 3)
                {
                    if (writeText && chatText.size() > 0)
                    {
                        if (menuPos3 == 0)
                            players[0].name = chatText;

                        if (menuPos3 == 1)
                            strcpy(sets->ipAddr, (char *)chatText.c_str());
                        if (menuPos3 == 2)
                            sets->tcpPort = atoi((char *)chatText.c_str());

                        chatText = "";
                    }
                    else
                    {
                        if (menuPos3 == 0)
                            chatText = players[0].name;
                        if (menuPos3 == 1)
                            chatText = sets->ipAddr;
                        if (menuPos3 == 2)
                        {
                            char buf[32] = {0};
                            sprintf(buf, "%u", sets->tcpPort);
                            chatText = buf;
                        }
                    }

                    writeText = !writeText;
                }
                else
                {
                    return 1;
                }
                continue;
            }

            if (!writeText)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                    return 1;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                {
                    addToSoundPool(SOUND_ID_hit4, 0, 0, 1.0);
                    menuPos3--;
                    if (menuPos3 < 0)
                        menuPos3 = 4;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                {
                    addToSoundPool(SOUND_ID_hit4, 0, 0, 1.0);
                    menuPos3++;
                    if (menuPos3 > 4)
                        menuPos3 = 0;
                }
            }
        }

        if (writeText)
        {
            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode == 8)
                {
                    if (chatText.size() > 0)
                    {
                        chatText.erase(chatText.begin() + chatText.size() - 1);
                    }
                }
                else
                {
                    if (event.text.unicode != 13)
                    {
                        if (event.text.unicode > 31)
                            chatText = chatText + static_cast<char>(event.text.unicode);
                    }
                }
            }
        }
    }

    sf::Vector2i pixelPos = sf::Mouse::getPosition(App);
    sf::Vector2f worldPos = mWindow.mapPixelToCoords(pixelPos);
    controls.mouseX = worldPos.x;
    controls.mouseY = worldPos.y;

    return 0;
}

int optionsInput(float dt)
{
    sf::Event event;

    while (App.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            return 0;

        if (!App.hasFocus())
            return SUCCESS;

        if (event.type == sf::Event::Resized)
        {
            sf::Vector2u si;
            si.x = event.size.width;
            si.y = event.size.height;

            sets->screenX = si.x;
            sets->screenY = si.y;

            initScreen();
            zoomTo(0);
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                return -1;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                return 1;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                return 3;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                return 4;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                addToSoundPool(SOUND_ID_hit4, 0, 0, 1.0);
                menuPos2--;
                if (menuPos2 < 0)
                    menuPos2 = 8;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                addToSoundPool(SOUND_ID_hit4, 0, 0, 1.0);
                menuPos2++;
                if (menuPos2 > 8)
                    menuPos2 = 0;
            }
        }
    }

    sf::Vector2i pixelPos = sf::Mouse::getPosition(App);
    sf::Vector2f worldPos = mWindow.mapPixelToCoords(pixelPos);
    controls.mouseX = worldPos.x;
    controls.mouseY = worldPos.y;

    return 0;
}

int get_input(float dt)
{
    sf::Event event;

    while (App.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            return 0;

        if (!App.hasFocus())
            return SUCCESS;

        if (event.type == sf::Event::Resized)
        {
            sf::Vector2u si;
            si.x = event.size.width;
            si.y = event.size.height;

            sets->screenX = si.x;
            sets->screenY = si.y;

            initScreen();
            zoomTo(0);
        }

        if (event.type == sf::Event::MouseWheelMoved)
        {
            if (event.mouseWheel.delta < 0) //scroll down
                zoomTo(-0.125 / 2.0);
            if (event.mouseWheel.delta > 0) //scroll up
                zoomTo(0.125 / 4.0);
        }

        if (event.type == sf::Event::KeyPressed)
        {
            //Esc is pressed, so set -1 to return. Its signal to go back/exit.
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                return -1;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
                showNames = !showNames;

            if (!Units[getMYID()].isAlive())
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
                {
                    cam.followEnt = 0;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
                {
                    if (playerAmount > 1)
                        cam.followEnt = 1;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
                {
                    if (playerAmount > 2)
                        cam.followEnt = 2;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
                {
                    if (playerAmount > 3)
                        cam.followEnt = 3;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
            {
                if (writeText && chatText.size() > 0)
                {
                    sendChatMSG((char *)players[0].name.c_str(), (char *)chatText.c_str());
                    char viesti[256] = {0};
                    sprintf(viesti, "MSG|%s|%s|", (char *)players[0].name.c_str(), (char *)chatText.c_str());
                    net.sendMsg(viesti);
                }

                writeText = !writeText;
                chatText = "";
                continue;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp))
                zoomTo((0.125));

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown))
                zoomTo((-0.125));
        }

        if (!writeText)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                controls.strafe_left = true;
            else
                controls.strafe_left = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                controls.strafe_right = true;
            else
                controls.strafe_right = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                controls.up = true;
            else
                controls.up = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                controls.down = true;
            else
                controls.down = false;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                controls.dropBom = true;
            else
                controls.dropBom = false;
        }

        if (writeText)
        {
            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode == 8)
                {
                    if (chatText.size() > 0)
                    {
                        chatText.erase(chatText.begin() + chatText.size() - 1);
                    }
                }
                else
                {
                    if (event.text.unicode != 13)
                    {
                        if (event.text.unicode > 31)
                            chatText = chatText + static_cast<char>(event.text.unicode);
                    }
                }
            }
        }
    }

    sf::Vector2i pixelPos = sf::Mouse::getPosition(App);
    sf::Vector2f worldPos = mWindow.mapPixelToCoords(pixelPos);
    controls.mouseX = worldPos.x;
    controls.mouseY = worldPos.y;

    return 0;
}

void drawText(std::string _text, int x, int y, int r, int g, int b, int a, int align, int toID)
{
    sf::Text &text = getTextfont(toID);

    text.setFont(getFont(toID));
    text.setString(_text);

    int align_x = 0;
    if (align == ALIGN_RIGHT)
        align_x = text.getLocalBounds().width;
    else if (align == ALIGN_CENTER)
        align_x = text.getLocalBounds().width / 2;

    text.setPosition(x - align_x, y);
    text.setColor(sf::Color((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a));
    App.draw(text);
}

void drawText3(std::string _text, int x, int y, int r, int g, int b, int a, int align, int toID, float size)
{
    sf::Text &text = getTextfont(toID);
    text.setFont(getFont(toID));

    text.setString(_text);
    text.setCharacterSize(size);
    text.setColor(sf::Color(0, 0, 0, a));

    int align_x = 0;
    int align_y = 0;
    if (align == ALIGN_RIGHT)
    {
        align_x = text.getLocalBounds().width;
    }
    else if (align == ALIGN_CENTER)
    {
        align_x = text.getLocalBounds().width / 2;
    }
    else if (align == ALIGN_CENTER_ALL)
    {
        align_x = text.getLocalBounds().width / 2;
        align_y = text.getLocalBounds().height / 2;
    }

    text.setPosition(x - 1 - align_x, y - align_y);
    mWindow.draw(text);

    text.setPosition(x + 1 - align_x, y - align_y);
    mWindow.draw(text);

    text.setPosition(x - align_x, y - 1 - align_y);
    mWindow.draw(text);

    text.setPosition(x - align_x, y + 1 - align_y);
    mWindow.draw(text);

    text.setPosition(x - align_x, y - align_y);
    text.setColor(sf::Color(r, g, b, a));
    mWindow.draw(text);
}

void drawText(std::string _text, int x, int y, int r, int g, int b, int a, int align, int toID, float size)
{
    sf::Text &text = getTextfont(toID);
    text.setFont(getFont(toID));
    

    text.setString(_text);
    text.setCharacterSize(size);
    text.setColor(sf::Color(0, 0, 0, a));

    int align_x = 0;
    int align_y = 0;
    if (align == ALIGN_RIGHT)
    {
        align_x = text.getLocalBounds().width;
    }
    else if (align == ALIGN_CENTER)
    {
        align_x = text.getLocalBounds().width / 2;
    }
    else if (align == ALIGN_CENTER_ALL)
    {
        align_x = text.getLocalBounds().width / 2;
        align_y = text.getLocalBounds().height / 2;
    }

    text.setPosition(x - 1 - align_x, y - align_y);
    App.draw(text);

    text.setPosition(x + 1 - align_x, y - align_y);
    App.draw(text);

    text.setPosition(x - align_x, y - 1 - align_y);
    App.draw(text);

    text.setPosition(x - align_x, y + 1 - align_y);
    App.draw(text);

    text.setPosition(x - align_x, y - align_y);
    text.setColor(sf::Color(r, g, b, a));
    App.draw(text);
}

void drawTextB(std::string _text, int x, int y, int r, int g, int b, int a, int align, int toID, float size)
{
    drawText(_text, x - 1, y, 0, 0, 0, a, align, toID, size);
    drawText(_text, x + 1, y, 0, 0, 0, a, align, toID, size);
    drawText(_text, x, y - 1, 0, 0, 0, a, align, toID, size);
    drawText(_text, x, y + 1, 0, 0, 0, a, align, toID, size);
    drawText(_text, x, y, r, g, b, a, align, toID, size);
}

void drawTextC(std::string _text, int x, int y, int r, int g, int b, int a, int align, int toID, float size)
{
    drawText3(_text, x - 1, y, 0, 0, 0, a, align, toID, size);
    drawText3(_text, x + 1, y, 0, 0, 0, a, align, toID, size);
    drawText3(_text, x, y - 1, 0, 0, 0, a, align, toID, size);
    drawText3(_text, x, y + 1, 0, 0, 0, a, align, toID, size);
    drawText3(_text, x, y, r, g, b, a, align, toID, size);
}

void renderLvl()
{
    sp.setTexture(getTexture(0));
    sp.setOrigin(tilex * 0.5, tiley * 0.5);

    float pX, pY;

    for (int y = 0; y < MAPY; y++)
    {
        for (int x = 0; x < MAPX; x++)
        {
            pX = int(((x * tilex) - (cam.dx * tilex)));
            pY = int(((y * tiley) - (cam.dy * tiley)));

            sp.setPosition(int(pX), int(pY));

            int tx = getLvl()->id[x][y] % 10;
            int ty = getLvl()->id[x][y] / 10;

            sp.setTextureRect(sf::IntRect(tilex * tx, tiley * ty, tilex, tiley));
            sp.setRotation(0);
            mWindow.draw(sp, sf::BlendAlpha);
        }
    }

    sp.setColor(sf::Color(255, 255, 255, 255));
    sp.setTexture(getTexture(2));
}

void renderLos()
{
    sp.setTexture(getTexture(3));
    sp.setOrigin(tilex * 0.5, tiley * 0.5);

    float pX, pY;

    for (int y = 0; y < MAPY; y++)
    {
        for (int x = 0; x < MAPX; x++)
        {
            int los = (int)getLvl()->los[x][y];

            if (los < 0)
                los = 0;
            if (los > 255)
                los = 255;

            pX = int((((x)*tilex) - (cam.dx * tilex))) - tilex;
            pY = int((((y)*tiley) - (cam.dy * tiley))) - tiley;

            sp.setPosition(int(pX), int(pY));

            sf::BlendMode blendmode = sf::BlendAlpha;
            blendmode.alphaEquation = sf::BlendMode::ReverseSubtract;

            if (los < 255)
            {
                sp.setColor(sf::Color(255, 255, 255, 255 - los));
                sp.setTextureRect(sf::IntRect(tilex * 0, tiley * 0, tilex * 3, tiley * 3));
                mWindow.draw(sp, blendmode);
            }
        }
    }

    sp.setColor(sf::Color(255, 255, 255, 255));
    sp.setTexture(getTexture(2));
}

void renderUnit(int i)
{
    sp.setTexture(getTexture(1));
    sp.setOrigin(tilex * 0.5 - 10, tiley * 0.5 - 10);

    float pX = int((int(Units[i].getdX() * tilex) - (cam.dx * tilex)));
    float pY = int((int(Units[i].getdY() * tiley) - (cam.dy * tiley)));

    sp.setColor(sf::Color(255, 255, 255, 255));
    sp.setPosition(pX, pY);
    int tx = 0;
    int ty = i;

    int dd = Units[i].getDirection();

    if (dd == 0)
        tx = 100;
    if (dd == 1)
        tx = 100 + (108);
    if (dd == 2)
        tx = 100 + (108 * 2);
    if (dd == 3)
        tx = 434;

    if (Units[i].getAnimFrame() != 3)
        tx += (Units[i].getAnimFrame() * 36);
    else
        tx += (1 * 36);

    if (!Units[i].isAlive())
        tx = 684 + (Units[i].getAnimFrame() * 34);

    if (readyCounter > 0)
        sp.setColor(sf::Color(0, 0, 0, 255));
    else
        sp.setColor(sf::Color(255, 255, 255, 255));

    sp.setTextureRect(sf::IntRect(tx, ty * 56, 36, 54));

    mWindow.draw(sp);

    sp.setOrigin(0, 0);
}

void renderUnits()
{
    for (unsigned int i = 0; i < playerAmount; i++)
        renderUnit(i);

    if (showNames)
    {
        for (unsigned int i = 0; i < playerAmount; i++)
        {
            float pX = int((int(Units[i].getdX() * tilex) - (cam.dx * tilex)));
            float pY = int((int(Units[i].getdY() * tiley) - (cam.dy * tiley)));
            Player &pl = getNamelist()->at(i);
            drawTextC(pl.name, pX, pY - 50, 255, 255, 255, 255, ALIGN_CENTER, 2, 16);
        }
    }
}

void renderItems()
{
    sp.setTexture(getTexture(2));
    sp.setOrigin(tilex * 0.5, tiley * 0.5);
    sp.setColor(sf::Color(255, 255, 255, 255));

    for (unsigned int i = 0; i < getLvl()->items.size(); i++)
    {
        float pX = int(((getLvl()->items[i].x * tilex) - (cam.dx * tilex)));
        float pY = int(((getLvl()->items[i].y * tiley) - (cam.dy * tiley)));

        if (getLvl()->id[getLvl()->items[i].x][getLvl()->items[i].y] != 1)
            continue;

        if (getLvl()->items[i].id == 30)
        {
            pX = int(((getLvl()->items[i].ddx * tilex) - (cam.dx * tilex)));
            pY = int(((getLvl()->items[i].ddy * tiley) - (cam.dy * tiley)));
        }

        sp.setPosition(pX, pY);

        int tx = 0;
        int ty = 0;
        int id = getLvl()->items[i].id;

        if (id == 0)
        {
            tx = 4 + (getLvl()->items[i].animFrame);
            ty = 0;
        }

        if (id == 1)
        {
            tx = 0;
            ty = 1;
        }

        if (id == 2)
        {
            tx = 1;
            ty = 1;
        }

        if (id == 3)
        {
            tx = 2;
            ty = 1;
        }

        if (id == 4)
        {
            tx = 3;
            ty = 1;
        }

        if (id == 5)
        {
            tx = 4;
            ty = 1;
        }

        if (id == 10)
        {
            tx = 7 + (getLvl()->items[i].animFrame);
            ty = 0;
        }

        if (id == 20)
        {
            tx = 5 + (getLvl()->items[i].animFrame);
            ty = 1;
        }

        if (id == 30)
        {
            tx = 8 + (getLvl()->items[i].animFrame);
            ty = 1;
        }

        sp.setTextureRect(sf::IntRect(tx * tilex, ty * tiley, tilex, tiley));
        mWindow.draw(sp);
    }

    sp.setColor(sf::Color(255, 255, 255, 255));

    for (unsigned int i = 0; i < getLvl()->explosives.size(); i++)
    {
        float pX = int(((getLvl()->explosives[i].x * tilex) - (cam.dx * tilex)));
        float pY = int(((getLvl()->explosives[i].y * tiley) - (cam.dy * tiley)));

        int a = 255;
        a = int(getLvl()->explosives[i].lifetime * 1000.0);
        if (a < 0)
            a = 0;
        if (a > 255)
            a = 255;

        sp.setColor(sf::Color(255, 255, 255, a));
        sp.setPosition(pX, pY);

        int tx = 0 + (getLvl()->explosives[i].tx + (getLvl()->explosives[i].animFrame * 3));
        int ty = 2 + (getLvl()->explosives[i].ty);

        sp.setTextureRect(sf::IntRect(tilex * tx, tiley * ty, tilex, tiley));
        mWindow.draw(sp);
    }

    sp.setOrigin(0, 0);
}

void loadingScreen()
{
    App.clear(); 
    drawText("LOADING", sets->screenX / 2, 48, 255, 255, 255, 255, ALIGN_CENTER, 0);
    App.display();
}

void drawPlayerStatus()
{
    for (unsigned int i = 0; i < playerAmount; i++)
    {
        Player &pl = getNamelist()->at(i);

        if (Units[i].isAlive())
            drawTextB(pl.name, sets->screenX - 48, 64 + (i * 32), 255, 255, 255, 255, ALIGN_RIGHT, 2, 20);
        else
            drawTextB(pl.name, sets->screenX - 48, 64 + (i * 32), 255, 64, 64, 255, ALIGN_RIGHT, 2, 20);

        sp.setOrigin(0, 0);
        sp.setTexture(getTexture(1));
        sp.setPosition(sets->screenX - 36, 56 + (i * 32));

        if (Units[i].isAlive())
            sp.setTextureRect(sf::IntRect(0, 56 * i, 32, 32));
        else
            sp.setTextureRect(sf::IntRect(32, 56 * i, 32, 32));

        App.draw(sp);
    }
}

void game_gui()
{
    char buf[32] = {0};
    sprintf(buf, "%0.2f", monitoring.fps);
    drawTextB(buf, 4, 4, 255, 255, 255, 255, ALIGN_LEFT, 0, 16.0);

    sprintf(buf, "%i", 30 - (seconds % 30));
    drawTextB(buf, sets->screenX - 48, 4, 255, 255, 255, 255, ALIGN_RIGHT, 0, 16.0);

    sprintf(buf, "cam: %0.2f / %0.2f", cam.dx, cam.dy);
    drawTextB(buf, 4 + 96, 4, 255, 255, 255, 255, ALIGN_LEFT, 0, 16.0);

    if (net.isConnected())
    {
        sprintf(buf, "ping: %0.4f", net.getPing());
        drawTextB(buf, 4 + 256, 4, 255, 255, 255, 255, ALIGN_LEFT, 0, 16.0);

        if (!Units[net.getMYID()].isAlive())
            drawTextB("DEFEATED", sets->screenX / 2, 64, 255, 64, 64, 255, ALIGN_CENTER, 0, 32.0);

        int abi = Units[net.getMYID()].getAbility();
        sp.setTexture(getTexture(2));
        sp.setOrigin(tilex * 0.5, tiley * 0.5);
        sp.setColor(sf::Color(255, 255, 255, 255));
        sp.setPosition(64, 64);
        int tx = 0;
        int ty = 0;

        if (abi == ABI_SPEED)
        {
            tx = 0;
            ty = 1;
        }
        if (abi == ABI_TIMER)
        {
            tx = 1;
            ty = 1;
        }
        if (abi == ABI_COOLDOWN)
        {
            tx = 2;
            ty = 1;
        }
        if (abi == ABI_MOVER)
        {
            tx = 3;
            ty = 1;
        }
        if (abi == ABI_RANGE)
        {
            tx = 4;
            ty = 1;
        }

        sp.setTextureRect(sf::IntRect(tx * tilex, ty * tiley, tilex, tiley));
        App.draw(sp);

        if (!Units[getMYID()].isAlive())
        {
            if (!winnerFound)
                drawTextB("PRESS 1, 2, 3 OR 4 TO FOLLOW PLAYER", sets->screenX / 2, sets->screenY - 128, 255, 255, 255, 255, ALIGN_CENTER, 0, 20.0);
        }
    }

    drawPlayerStatus();

    winners = 0;
    int seuraaVoittajaa = -1;
    for (unsigned int i = 0; i < playerAmount; i++)
    {
        if (Units[i].isAlive())
        {
            winners++;
            seuraaVoittajaa = i;
        }
    }

    if (winCounter <= 0.0)
    {
        if (winners == 1)
        {
            if (!winnerFound)
            {
                winCounter = 5.0;
                winnerFound = true;
                cam.followEnt = seuraaVoittajaa;
                cam.movingSpeed = 0.25;

                if (seuraaVoittajaa == getMYID())
                {
                    addToSoundPool(SOUND_ID_hit1, 0, 0, 1.0);
                }
            }
        }
        else if (winners == 0)
        {
            winCounter = 7.0;
            winnerFound = true;
        }
    }
}

void drawRect(DRECT rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    sf::RectangleShape rectangle(sf::Vector2f(rect.x, rect.y));
    rectangle.setPosition(rect.x, rect.y);
    rectangle.setRotation(0);
    rectangle.setSize(sf::Vector2f(rect.w, rect.h));
    rectangle.setOutlineThickness(1.0f);
    rectangle.setOutlineColor(sf::Color(0, 0, 0, 255));
    rectangle.setFillColor(sf::Color(r, g, b, a));
    App.draw(rectangle);
}

void renderChat(int posX, int posY)
{
    
    int sizeOfChat = getChat()->msgs.size();
    int last_i = sizeOfChat - 10;

    DRECT r;
    r.x = posX;
    r.y = posY;
    r.w = 480;
    r.h = 180;
    drawRect(r, 0, 0, 0, 64);

    r.y += 182;
    r.h = 24;
    drawRect(r, 0, 0, 0, 64);

    for (int i = 0; i < 10; i++)
    {
        int messageIDs = last_i + i;
        if (messageIDs >= 0)
        {
            drawText(getChat()->msgs[messageIDs].sender, posX + 16, posY + (i * 16), 255, 255, 255, 255, ALIGN_LEFT, 0, 20);
            drawText(getChat()->msgs[messageIDs].msg, posX + 128, posY + (i * 16), 255, 255, 255, 255, ALIGN_LEFT, 0, 20);
        }
    }
    

    if (writeText)
        drawText(chatText + "_", posX + 50, posY + (11 * 16) + 4, 255, 255, 255, 255, ALIGN_LEFT, 0, 20);
    else
        drawText(chatText, posX + 50, posY + (11 * 16) + 4, 255, 255, 255, 255, ALIGN_LEFT, 0, 20);
}

void render()
{
    App.clear(); 
    mWindow.clear(sf::Color(64, 96, 192));

    renderLvl();
    renderItems();
    renderUnits();

    if (getLvl()->rules.los)
        renderLos();

    mWindow.display();
    const sf::Texture &texturer = mWindow.getTexture();
    sf::Sprite sprite(texturer);
    App.draw(sprite);
    renderChat(16, sets->screenY - 216);
    game_gui();

    if (readyCounter >= 1.0)
    {
        char lukk[32] = {0};
        sprintf(lukk, "%i", (int)readyCounter);

        int r, g, b;
        r = 0;
        g = 255;
        b = 0;

        if (readyCounter > 1.0)
        {
            r = 160;
            g = 255;
            b = 0;
        }
        if (readyCounter > 2.0)
        {
            r = 255;
            g = 255;
            b = 0;
        }
        if (readyCounter > 3.0)
        {
            r = 255;
            g = 160;
            b = 0;
        }
        if (readyCounter > 4.0)
        {
            r = 255;
            g = 0;
            b = 0;
        }

        drawTextB(lukk, sets->screenX / 2, sets->screenY / 2, r, g, b, 255, ALIGN_CENTER_ALL, 0, 20 + (secTimer * 32.0));
    }
    else if (readyCounter > 0)
    {
        int rr = (int)((secTimer)*255);
        drawTextB("GO!", sets->screenX / 2, (sets->screenY / 2), 255, 255, 255, (rr), ALIGN_CENTER_ALL, 0, 20 + ((1 - secTimer) * 90.0));
    }

    if (winnerFound)
    {
        char buf[128] = {0};
        if (winners == 1)
        {
            Player &pl = getNamelist()->at(cam.followEnt);
            sprintf(buf, "WINNER: %s", (char *)pl.name.c_str());
        }
        else
        {
            sprintf(buf, "TIE");
        }

        drawTextB(buf, sets->screenX / 2, sets->screenY / 2 + 128, 255, 255, 255, 255, ALIGN_CENTER_ALL, 0, 40.0);
    }

    App.display(); 
}

void menuRender()
{
    App.clear();
    mWindow.clear(sf::Color(64, 96, 192));
    mWindow.display();
    const sf::Texture &texturer = mWindow.getTexture();
    sf::Sprite sprite(texturer);
    App.draw(sprite);

    int r, g, b;
    r = g = b = 255;
    drawTextB(ApplicationName, sets->screenX / 2, 128, r, g, b, 255, ALIGN_CENTER, 2, 32);

    for (int i = 0; i < 5; i++)
    {
        r = g = b = 128;

        if (i == menuPos)
        {
            r = g = b = 255;
        }

        int kk = sets->screenY * 0.5;

        if (i == 0)
            drawTextB("CREATE CHARACTER", 128, kk, r, g, b, 255, ALIGN_LEFT, 2, 32);

        if (i == 1)
            drawTextB("HOST", 128, kk + 32, r, g, b, 255, ALIGN_LEFT, 2, 32);

        if (i == 2)
            drawTextB("JOIN", 128, kk + 64, r, g, b, 255, ALIGN_LEFT, 2, 32);

        if (i == 3)
            drawTextB("SETTINGS", 128, kk + 96, r, g, b, 255, ALIGN_LEFT, 2, 32);

        if (i == 4)
            drawTextB("BACK", 128, kk + 128, r, g, b, 255, ALIGN_LEFT, 2, 32);
    }

    App.display();
}

void searchRender(bool asiakas)
{
    App.clear();
    mWindow.clear(sf::Color(64, 96, 192));
    mWindow.display();
    const sf::Texture &texturer = mWindow.getTexture();
    sf::Sprite sprite(texturer);
    App.draw(sprite);

    int r, g, b;
    r = g = b = 255;
    if (asiakas)
        drawTextB("SEARCH LOBBY", sets->screenX / 2, 128, r, g, b, 255, ALIGN_CENTER, 2, 32);
    else
        drawTextB("CREATE LOBBY", sets->screenX / 2, 128, r, g, b, 255, ALIGN_CENTER, 2, 32);

    for (int i = 0; i < 5; i++)
    {
        r = g = b = 128;
        if (i == menuPos3)
        {
            r = g = b = 255;
        }

        int kk = sets->screenY * 0.5;
        char tei[64] = {0};

        int rr = 2;
        int ff = 20;

        if (i == 0)
        {
            drawTextB("PLAYERNAME", 128, kk, r, g, b, 255, ALIGN_LEFT, rr, ff);
            sprintf(tei, "%s", (char *)players[0].name.c_str());

            if (writeText && menuPos3 == 0)
            {
                sprintf(tei, "%s_", (char *)chatText.c_str());
            }

            drawTextB(tei, 320, kk, r, g, b, 255, ALIGN_LEFT, rr, ff);
        }

        if (i == 1)
        {
            drawTextB("IP ADDR", 128, kk + ff, r, g, b, 255, ALIGN_LEFT, rr, ff);
            sprintf(tei, "%s", sets->ipAddr);

            if (writeText && menuPos3 == 1)
            {
                sprintf(tei, "%s_", (char *)chatText.c_str());
            }

            drawTextB(tei, 320, kk + ff, r, g, b, 255, ALIGN_LEFT, rr, ff);
        }

        if (i == 2)
        {
            drawTextB("PORT", 128, kk + (ff * 2), r, g, b, 255, ALIGN_LEFT, rr, ff);
            sprintf(tei, "%u", sets->tcpPort);

            if (writeText && menuPos3 == 2)
            {
                sprintf(tei, "%s_", (char *)chatText.c_str());
            }

            drawTextB(tei, 320, kk + (ff * 2), r, g, b, 255, ALIGN_LEFT, rr, ff);
        }
        if (i == 3)
        {
            if (asiakas)
                drawTextB("CONNECT", 128, kk + (ff * 3), r, g, b, 255, ALIGN_LEFT, rr, ff);
            else
                drawTextB("CREATE SERVER", 128, kk + (ff * 3), r, g, b, 255, ALIGN_LEFT, rr, ff);
        }

        if (i == 4)
        {
            drawTextB("BACK", 128, kk + (ff * 4), r, g, b, 255, ALIGN_LEFT, rr, ff);
        }
    }

    App.display();
}

void optionsRender()
{
    App.clear();
    mWindow.clear(sf::Color(64, 96, 192));
    mWindow.display();
    const sf::Texture &texturer = mWindow.getTexture();
    sf::Sprite sprite(texturer);
    App.draw(sprite);

    int r, g, b;
    r = g = b = 255;

    drawTextB("SETTINGS", sets->screenX / 2, 128, r, g, b, 255, ALIGN_CENTER, 2, 26);

    for (int i = 0; i < 9; i++)
    {
        r = g = b = 128;

        if (i == menuPos2)
        {
            r = g = b = 255;
        }

        int kk = sets->screenY * 0.5;

        if (i == 0)
            drawTextB("RESOLUTION", 128, kk, r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 1)
            drawTextB("FULLSCREEN", 128, kk + (20), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 2)
            drawTextB("FRAMECAP", 128, kk + (20 * 2), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 3)
            drawTextB("VSYNC", 128, kk + (20 * 3), r, g, b, 255, ALIGN_LEFT, 2, 20);

        if (i == 4)
            drawTextB("MASTER VOLUME", 128, kk + (20 * 5), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 5)
            drawTextB("MUSIC", 128, kk + (20 * 6), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 6)
            drawTextB("SOUND", 128, kk + (20 * 7), r, g, b, 255, ALIGN_LEFT, 2, 20);

        if (i == 7)
            drawTextB("FONT SCALE", 128, kk + (20 * 9), r, g, b, 255, ALIGN_LEFT, 2, 20);

        if (i == 8)
            drawTextB("BACK", 128, kk + (20 * 11), r, g, b, 255, ALIGN_LEFT, 2, 20);

        char tek[128] = {0};

        if (i == 0)
            sprintf(tek, "%ix%i", sets->screenX, sets->screenY);
        if (i == 2)
            sprintf(tek, "%i", sets->framelimit);

        if (i == 4)
            sprintf(tek, "%i%%", (int)(sets->masterVolume * 100));
        if (i == 5)
            sprintf(tek, "%i%%", (int)(sets->musicVolume * 100));
        if (i == 6)
            sprintf(tek, "%i%%", (int)(sets->soundVolume * 100));
        if (i == 7)
            sprintf(tek, "%0.2f", sets->fontSize);

        if (i == 0)
            drawTextB(tek, 320, kk, r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 1 && sets->fullscreen)
            drawTextB("ON", 320, kk + (20), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 1 && !sets->fullscreen)
            drawTextB("OFF", 320, kk + (20), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 2)
            drawTextB(tek, 320, kk + (20 * 2), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 3 && sets->vsync)
            drawTextB("ON", 320, kk + (20 * 3), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 3 && !sets->vsync)
            drawTextB("OFF", 320, kk + (20 * 3), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 4)
            drawTextB(tek, 320, kk + (20 * 5), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 5)
            drawTextB(tek, 320, kk + (20 * 6), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 6)
            drawTextB(tek, 320, kk + (20 * 7), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 7)
            drawTextB(tek, 320, kk + (20 * 9), r, g, b, 255, ALIGN_LEFT, 2, 20);
    }

    App.display();
}

int quit()
{
    App.close();
    saveSettings();
    deinit();
    return 0;
}

void sendPos(int id)
{
    char buf[127] = {0};
    sprintf(buf, "POS|%i|%i|%i|%i|", id, (int)Units[id].getX(), (int)Units[id].getY(), Units[id].getDirection());
    net.sendUDP(buf);
}

void setUnitPos(int id, int x, int y, int direction)
{
    Units[id].setPosForce2(x, y);
    Units[id].forceAnim(2);
    Units[id].setDirection(direction);
}

void dropBom(int x, int y, int tyyppi, int ownerID)
{
    addToSoundPool(SOUND_ID_death1, x, y, 1.0);

    if (net.isServer())
        addItem(x, y, tyyppi, ownerID);

    char buf[127] = {0};
    sprintf(buf, "ADDITEM|%i|%i|%i|%i|", tyyppi, x, y, ownerID);
    net.sendMsg(buf);
}

void destroyUnitsHere(int x, int y)
{
    for (unsigned int i = 0; i < playerAmount; i++)
    {
        if ((int)Units[i].getX() == x && (int)Units[i].getY() == y)
        {
            if ((int)i == getMYID())
            {
                Units[i].setAlive(false);
                char buf[127] = {0};
                sprintf(buf, "DEAD|%i|%s|", getMYID(), (char *)players[0].name.c_str());
                net.sendMsg(buf);
                addToSoundPool(SOUND_ID_death2, x, y, 1.0);
            }
        }
    }
}

void createExplosion(int x, int y, int koko)
{
    addToSoundPool(SOUND_ID_explo01, x, y, 1.0);

    Explo xp;

    xp.animCounter = 0;
    xp.animFrame = 0;
    xp.animSpeed = 0.125 * 0.5;
    xp.tx = 1;
    xp.ty = 1;
    xp.lifetime = 0.25 * 2.0;
    xp.x = x;
    xp.y = y;

    getLvl()->explosives.push_back(xp);
    destroyUnitsHere(x, y);

    for (int i = 1; i < koko; i++)
    {
        xp.x = x + i;
        xp.y = y;

        xp.tx = 1;
        xp.ty = 1;

        if (i == koko - 1)
        {
            xp.tx = 2;
        }

        if (getLvl()->id[x + i][y] == 2)
        {
            getLvl()->id[x + i][y] = 1;
            break;
        }
        if (getLvl()->id[x + i][y] == 0)
        {
            break;
        }

        getLvl()->explosives.push_back(xp);
        destroyUnitsHere(x + i, y);
    }

    for (int i = 1; i < koko; i++)
    {
        xp.x = x - i;
        xp.y = y;

        xp.tx = 1;
        xp.ty = 1;

        if (i == koko - 1)
        {
            xp.tx = 0;
        }

        if (getLvl()->id[x - i][y] == 2)
        {
            getLvl()->id[x - i][y] = 1;
            break;
        }
        if (getLvl()->id[x - i][y] == 0)
        {
            break;
        }

        getLvl()->explosives.push_back(xp);
        destroyUnitsHere(x - i, y);
    }

    for (int i = 1; i < koko; i++)
    {
        xp.x = x;
        xp.y = y - i;
        xp.tx = 1;
        xp.ty = 1;

        if (i == koko - 1)
        {
            xp.ty = 0;
        }

        if (getLvl()->id[x][y - i] == 2)
        {
            getLvl()->id[x][y - i] = 1;
            break;
        }
        if (getLvl()->id[x][y - i] == 0)
        {
            break;
        }

        getLvl()->explosives.push_back(xp);
        destroyUnitsHere(x, y - i);
    }

    for (int i = 1; i < koko; i++)
    {
        xp.x = x;
        xp.y = y + i;
        xp.tx = 1;
        xp.ty = 1;

        if (i == koko - 1)
        {
            xp.ty = 2;
        }

        if (getLvl()->id[x][y + i] == 2)
        {
            getLvl()->id[x][y + i] = 1;
            break;
        }
        if (getLvl()->id[x][y + i] == 0)
        {
            break;
        }

        getLvl()->explosives.push_back(xp);
        destroyUnitsHere(x, y + i);
    }
}

void destroyMyBomb(int id)
{
    for (unsigned int i = 0; i < getLvl()->items.size(); i++)
    {
        if (getLvl()->items[i].ownerID == id)
        {
            getLvl()->items[i].lifetime = 0.0;
        }
    }
}


//every 30 seconds decrease map size. Its like BR.
void decreaseMap()
{
    int a = (seconds / 30);

    for (int y = a; y < MAPY - (a); y++)
    {
        for (int x = a; x < MAPX - (a); x++)
        {
            if (y == a || y == MAPY - (1 + a) || x == a || x == MAPX - (1 + a))
            {
                destroyUnitsHere(x, y);
                getLvl()->id[x][y] = 0;
            }
        }
    }
}

int isthereUnit(int x, int y)
{
    for (unsigned int i = 0; i < playerAmount; i++)
    {
        if (Units[i].getX() == x && Units[i].getY() == y)
            return i;
    }
    return -1;
}

int do_logic(float deltaTimer)
{
    float pX = Units[net.getMYID()].getX();
    float pY = Units[net.getMYID()].getY();
    int ii = net.getMYID(); //our player's id.

    bool move = false;
    for (unsigned int i = 0; i < playerAmount; i++)
    {
        if (Units[i].isAlive())
            Units[i].playAnimation(false);
        else
            Units[i].playAnimation(true);
    }

    //disable controls if winners found
    if (winnerFound)
    {
        controls.strafe_left = false;
        controls.strafe_right = false;
        controls.up = false;
        controls.down = false;
        controls.dropBom = false;
    }

    int lx = 0;
    int ly = 0;

    //try move only our player to left
    if (controls.strafe_left)
    {
        pX -= 1;
        lx = -1;
        Units[ii].setDirection((Directions)LEFT);
        Units[ii].playAnimation(true);
        move = true;
    }
    //try move only our player to right
    else if (controls.strafe_right)
    {
        pX += 1;
        lx = 1;
        Units[ii].setDirection((Directions)RIGHT);
        Units[ii].playAnimation(true);
        move = true;
    }
    //try move only our player to up
    else if (controls.up)
    {
        pY -= 1;
        ly = -1;
        Units[ii].setDirection((Directions)UP);
        Units[ii].playAnimation(true);
        move = true;
    }
    //try move only our player to down
    else if (controls.down)
    {
        pY += 1;
        ly = 1;
        Units[ii].setDirection((Directions)DOWN);
        Units[ii].playAnimation(true);
        move = true;
    }

    for (unsigned int i = 0; i < playerAmount; i++)
    {
        Units[i].doLogic(deltaTimer);
    }

    if (!isCollide((int)pX, (int)pY))
    {
        Units[net.getMYID()].setPos(pX, pY);
    }
    else
    {
        //if item in position
        int itemID = searchItem((int)pX, (int)pY);

        if (itemID != -1)
        {
            if (getLvl()->items[itemID].id == 30)
            {
                char moveObj[32] = {0};
                sprintf(moveObj, "ITEMDIR|%i|%i|%i|", getLvl()->items[itemID].itemID, lx, ly);
                net.sendMsg(moveObj);

                changeItemDirection(getLvl()->items[itemID].itemID, lx, ly);
            }
        }
    }

    static float counter = 0.0;

    counter += deltaTimer;
    //send 20 messages per second to server
    if (counter >= 1.0 / 20.0)
    {
        ////send only if move
        if (move) 
        {
            sendPos(net.getMYID());
        }

        //remove 0.05 sec from counter, because its 1 from tickrate.
        counter -= 1.0 / 20.0;

        if (Units[net.getMYID()].isAlive())
        {
            if (controls.dropBom)
            {
                int lx = Units[net.getMYID()].getdX() + 0.5;
                int ly = Units[net.getMYID()].getdY() + 0.5;

                controls.dropBom = false;
                if (Units[net.getMYID()].bombCooldown <= 0.0)
                {
                    if (!isCollide((int)lx, (int)ly))
                    {
                        if (Units[net.getMYID()].getAbility() == ABI_RANGE)
                        {
                            dropBom((int)lx, (int)ly, 10, net.getMYID()); //pommityyppi
                        }
                        else if (Units[net.getMYID()].getAbility() == ABI_MOVER)
                        {
                            dropBom((int)lx, (int)ly, 30, net.getMYID()); //pommityyppi
                        }
                        else if (Units[net.getMYID()].getAbility() == ABI_TIMER)
                        {
                            if (!Units[net.getMYID()].isBombDropped())
                            {
                                dropBom((int)lx, (int)ly, 20, net.getMYID()); //pommityyppi
                                Units[net.getMYID()].setBombDropped(true);
                                printf("dropped.\n");
                            }
                            else
                            {
                                Units[net.getMYID()].setBombDropped(false);
                                destroyMyBomb(net.getMYID());
                                char vii[32] = {0};
                                sprintf(vii, "BOMB|%i|", getMYID());
                                net.sendMsg(vii);
                            }
                        }
                        else
                        {
                            dropBom((int)lx, (int)ly, 0, net.getMYID()); //pommityyppi
                        }
                    }

                    Units[net.getMYID()].bombCooldown = 1.0;

                    if (Units[net.getMYID()].getAbility() == ABI_COOLDOWN)
                    {
                        Units[net.getMYID()].bombCooldown = 0.5;
                    }
                }
            }
        }
    }

    for (int i = getLvl()->items.size() - 1; i >= 0; i--)
    {
        if (getLvl()->items[i].id == 0 || getLvl()->items[i].id == 10 || getLvl()->items[i].id == 30)
        {
            getLvl()->items[i].animCounter += deltaTimer;

            if (getLvl()->items[i].animCounter >= getLvl()->items[i].animSpeed)
            {
                getLvl()->items[i].animCounter -= getLvl()->items[i].animSpeed;
                getLvl()->items[i].animFrame++;

                if (getLvl()->items[i].animFrame > 2)
                    getLvl()->items[i].animFrame = 0;

                if (getLvl()->items[i].id == 30)
                {
                    if (!isCollide((int)getLvl()->items[i].x + (int)getLvl()->items[i].dx, (int)getLvl()->items[i].y + (int)getLvl()->items[i].dy))
                    {
                        int uid = isthereUnit((int)getLvl()->items[i].x + (int)getLvl()->items[i].dx, (int)getLvl()->items[i].y + (int)getLvl()->items[i].dy);

                        if (uid == -1 || uid == getMYID())
                        {
                            getLvl()->items[i].x += (int)getLvl()->items[i].dx;
                            getLvl()->items[i].y += (int)getLvl()->items[i].dy;
                        }
                        else
                        {
                            getLvl()->items[i].dx = 0;
                            getLvl()->items[i].dy = 0;
                        }
                    }
                    else
                    {
                        getLvl()->items[i].dx = 0;
                        getLvl()->items[i].dy = 0;
                    }
                }
                else
                {
                    getLvl()->items[i].dx = 0;
                    getLvl()->items[i].dy = 0;
                }
            }
        }

        if (getLvl()->items[i].id == 20)
        {
            getLvl()->items[i].animCounter += deltaTimer * 0.5;

            if (getLvl()->items[i].animCounter >= getLvl()->items[i].animSpeed)
            {
                getLvl()->items[i].animCounter -= getLvl()->items[i].animSpeed;
                getLvl()->items[i].animFrame++;

                if (getLvl()->items[i].animFrame > 2)
                    getLvl()->items[i].animFrame = 0;
            }
        }

        if (getLvl()->items[i].id == 0)
        {
            if (getLvl()->items[i].lifetime >= 0)
            {
                getLvl()->items[i].lifetime -= deltaTimer;
            }
            else
            {
                createExplosion(getLvl()->items[i].x, getLvl()->items[i].y, 3);
                getLvl()->items.erase(getLvl()->items.begin() + i);
            }
        }
        if (getLvl()->items[i].id == 10)
        {
            if (getLvl()->items[i].lifetime >= 0)
            {
                getLvl()->items[i].lifetime -= deltaTimer;
            }
            else
            {
                createExplosion(getLvl()->items[i].x, getLvl()->items[i].y, 5);
                getLvl()->items.erase(getLvl()->items.begin() + i);
            }
        }
        if (getLvl()->items[i].id == 20)
        {
            if (getLvl()->items[i].lifetime >= 0)
            {
                getLvl()->items[i].lifetime -= deltaTimer;
            }
            else
            {
                createExplosion(getLvl()->items[i].x, getLvl()->items[i].y, 2);
                getLvl()->items.erase(getLvl()->items.begin() + i);
            }
        }
        if (getLvl()->items[i].id == 30)
        {
            if (getLvl()->items[i].lifetime >= 0)
            {
                getLvl()->items[i].lifetime -= deltaTimer;
            }
            else
            {
                createExplosion(getLvl()->items[i].x, getLvl()->items[i].y, 4);
                getLvl()->items.erase(getLvl()->items.begin() + i);
            }
        }
    }

    for (int i = getLvl()->explosives.size() - 1; i >= 0; i--)
    {
        if (getLvl()->explosives[i].lifetime >= 0)
        {
            getLvl()->explosives[i].lifetime -= deltaTimer;
            getLvl()->explosives[i].animCounter += deltaTimer;

            if (getLvl()->explosives[i].animFrame < 4)
            {
                if (getLvl()->explosives[i].animCounter >= getLvl()->explosives[i].animSpeed)
                {
                    getLvl()->explosives[i].animCounter -= getLvl()->explosives[i].animSpeed;
                    getLvl()->explosives[i].animFrame++;
                }
            }
        }
        else
        {
            getLvl()->explosives.erase(getLvl()->explosives.begin() + i);
        }
    }

    for (unsigned int i = 0; i < getLvl()->items.size(); i++)
    {
        if (getLvl()->items[i].id == 30)
        {
            float dtx = (getLvl()->items[i].ddx - (float)getLvl()->items[i].x) / 0.2;
            float dty = (getLvl()->items[i].ddy - (float)getLvl()->items[i].y) / 0.2;

            getLvl()->items[i].ddx -= dtx * deltaTimer;
            getLvl()->items[i].ddy -= dty * deltaTimer;
        }
    }

    for (unsigned int i = 0; i < playerAmount; i++)
    {
        for (unsigned int j = 0; j < getLvl()->items.size(); j++)
        {
            if ((int)Units[i].getX() == (int)getLvl()->items[j].x)
            {
                if ((int)Units[i].getY() == (int)getLvl()->items[j].y)
                {
                    int abi = getLvl()->items[j].id;

                    if (abi != 0 && abi != 10 && abi != 20 && abi != 30)
                    {
                        Units[i].setAbility(abi);
                        getLvl()->items.erase(getLvl()->items.begin() + j);
                        addToSoundPool(SOUND_ID_AMMO, Units[i].getX(), Units[i].getY(), 1.0);
                    }
                    break;
                }
            }
        }
    }

    if (getLvl()->rules.los)
    {
        int lx = Units[cam.followEnt].getX();
        int ly = Units[cam.followEnt].getY();
        calcLos(lx + 0.5, ly + 0.5, deltaTimer);
    }

    float eX, eY;
    if (cam.followEnt >= 0)
    {
        eX = (cam.dx - (Units[cam.followEnt].getdX() - ((sets->screenX / tilex) * 0.5))) / cam.movingSpeed * deltaTimer;
        eY = (cam.dy - (Units[cam.followEnt].getdY() - ((sets->screenY / tiley) * 0.5))) / cam.movingSpeed * deltaTimer;

        cam.dx -= eX;
        cam.dy -= eY;
    }

    gameCounter += deltaTimer;

    if (!winnerFound)
    {
        if (gameCounter >= 1.0)
        {
            gameCounter -= 1.0;
            seconds++;

            if (seconds % 30 == 0)
            {
                decreaseMap();
            }
        }
    }

    return 0;
}

//the main game loop
//not really complex.
int gameloop()
{
    float dt = 0.0; //delta timer

    //create level
    initLevel(getSeed());

    //need to know how many players in match
    playerAmount = getNamelist()->size();

    //set all players to right positions and alive.
    for (unsigned int i = 0; i < playerAmount; i++)
    {
        if (i == 0)
            Units[i].setPosForce(1, 1);
        if (i == 1)
            Units[i].setPosForce(MAPX - 2, 1);
        if (i == 2)
            Units[i].setPosForce(MAPX - 2, MAPY - 2);
        if (i == 3)
            Units[i].setPosForce(1, MAPY - 2);

        getLvl()->id[1][1] = 1;
        getLvl()->id[1][MAPY - 2] = 1;
        getLvl()->id[MAPX - 2][MAPY - 2] = 1;
        getLvl()->id[MAPX - 2][1] = 1;

        Units[i].setAlive(true);
        Units[i].setAbility(ABI_NONE);
    }

    //our player. Ensure its alive.
    Units[getMYID()].setAlive(true);

    winners = 0;
    //we want camera follow our player, so send coordinates to camera.
    setCameraFollow(net.getMYID(),
                    Units[getMYID()].getX(),
                    Units[getMYID()].getY(),
                    0.1);

    

    sf::Clock clock;
    clock.restart();
    float timeCounter = 0;

    if (getLvl()->rules.los)
    {
        clearLos();
    }

    readyCounter = 5.0;
    secTimer = 1.0;
    winCounter = 0.0;
    winnerFound = false;
    seconds = 0;
    gameCounter = 0;

    addToSoundPool(SOUND_ID_warmup, 0, 0, 1.0);
   
    while (1)
    {

        /*
        if (net.isConnected()) {
            net.listen();
        } else {
            break;
        }
        */

        //harcoded tickrate.
        float tickrate = 30.0;


        if (readyCounter <= 0.0)
        {
            if (get_input(dt) != 0)
                break;

            if (gameStatus == G_MAINMENU)
                break;

            if (dt >= 1.0 / tickrate)
            {
                while (1)
                {
                    dt -= (1.0 / tickrate);
                    do_logic(1.0 / tickrate);

                    if (dt < 1.0 / tickrate)
                    {
                        break;
                    }
                }
            }

            do_logic(dt);
        }
        else
        {
            readyCounter -= dt;
            float eX, eY;

            if (cam.followEnt >= 0)
            {
                eX = (cam.dx - (Units[cam.followEnt].getdX() - ((sets->screenX / tilex) * 0.5))) / (cam.movingSpeed * 100.0) * dt;
                eY = (cam.dy - (Units[cam.followEnt].getdY() - ((sets->screenY / tiley) * 0.5))) / (cam.movingSpeed * 100.0) * dt;

                cam.dx -= eX;
                cam.dy -= eY;
            }
        }

        render();

        dt = clock.getElapsedTime().asSeconds();
        clock.restart();
        timeCounter += dt;

        if (timeCounter >= 1.0)
        {
            timeCounter -= 1.0;
            secTimer = 1.0;

            if (winCounter > 0)
            {
                for (unsigned int a = 0; a < playerAmount; a++)
                {
                    int dd = Units[a].getDirection();
                    dd++;
                    if (dd > 3)
                        dd = 0;
                    Units[a].setDirection(dd);
                }
            }
            else
            {
                if (winnerFound)
                    break;
            }
        }
        secTimer -= dt;

        monitoring.deltatimer = dt;
        monitoring.fps = 1.0 / dt;

        if (winCounter > 0)
            winCounter -= dt;

        if (winCounter <= 0)
            winCounter = 0.0;
    }

    readyCounter = 5.0;
    secTimer = 1.0;
    gameStatus = G_MAINMENU;

    //back to lobby, so send UNREADY to everyone.
    if (net.isServer())
    {
        net.sendMsg((char *)"UNREADY|HOST|");
    }

    return 0;
}

void renderLobby()
{
    App.clear();
    mWindow.clear(sf::Color(64, 96, 192));
    mWindow.display();
    const sf::Texture &texturer = mWindow.getTexture();
    sf::Sprite sprite(texturer);
    App.draw(sprite);

    int r, g, b;
    r = g = b = 255;

    int kk = sets->screenY * 0.1;

    drawTextB("LOBBY", 128, kk, r, g, b, 255, ALIGN_LEFT, 2, 32);

    for (unsigned int i = 0; i < getNamelist()->size(); i++)
    {
        Player &pl = getNamelist()->at(i);
        if (pl.ready)
            drawTextB(pl.name, sets->screenX - 64, kk + 200 + (i * 20), 255, 255, 255, 255, ALIGN_RIGHT, 2, 20);
        else
            drawTextB(pl.name, sets->screenX - 64, kk + 200 + (i * 20), 128, 128, 128, 255, ALIGN_RIGHT, 2, 20);
    }

    int xx = 192;

    drawTextB("SETTINGS", sets->screenX - xx, kk - 32, r, g, b, 255, ALIGN_LEFT, 2, 20);

    for (int i = 0; i < 6; i++)
    {
        if (i == 0)
        {
            if (getLvl()->rules.itemSpeed)
                r = g = b = 255;
            else
                r = g = b = 128;
        }
        if (i == 1)
        {
            if (getLvl()->rules.itemTimer)
                r = g = b = 255;
            else
                r = g = b = 128;
        }
        if (i == 2)
        {
            if (getLvl()->rules.itemCooldown)
                r = g = b = 255;
            else
                r = g = b = 128;
        }
        if (i == 3)
        {
            if (getLvl()->rules.itemMover)
                r = g = b = 255;
            else
                r = g = b = 128;
        }
        if (i == 4)
        {
            if (getLvl()->rules.itemRange)
                r = g = b = 255;
            else
                r = g = b = 128;
        }

        if (i == 5)
        {
            if (getLvl()->rules.los)
                r = g = b = 255;
            else
                r = g = b = 128;
        }

        if (i == 0)
            drawTextB("1) SPEED", sets->screenX - xx, kk + (i * 20), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 1)
            drawTextB("2) TIMERS", sets->screenX - xx, kk + (i * 20), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 2)
            drawTextB("3) FAST BOMBS", sets->screenX - xx, kk + (i * 20), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 3)
            drawTextB("4) PUSHER", sets->screenX - xx, kk + (i * 20), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 4)
            drawTextB("5) BIG BOMBS", sets->screenX - xx, kk + (i * 20), r, g, b, 255, ALIGN_LEFT, 2, 20);
        if (i == 5)
            drawTextB("6) FOV", sets->screenX - xx, kk + (i * 20), r, g, b, 255, ALIGN_LEFT, 2, 20);
    }

    drawTextB("READY? PRESS F1", 128, kk + (2 * 20), 255, 255, 255, 255, ALIGN_LEFT, 2, 20);

    renderChat(16, 256);
    App.display();
}

void sendRules()
{
    char buf[64] = {0};
    sprintf(buf, "RULES|%i|%i|%i|%i|%i|%i|",
            getLvl()->rules.los,
            getLvl()->rules.itemCooldown,
            getLvl()->rules.itemMover,
            getLvl()->rules.itemRange,
            getLvl()->rules.itemSpeed,
            getLvl()->rules.itemTimer);

    net.sendMsg(buf);
}

int lobbyInput(float dt)
{
    sf::Event event;

    while (App.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            return -1;

        if (!App.hasFocus())
            return SUCCESS;

        if (event.type == sf::Event::Resized)
        {
            sf::Vector2u si;
            si.x = event.size.width;
            si.y = event.size.height;

            sets->screenX = si.x;
            sets->screenY = si.y;

            initScreen();
            zoomTo(0);
        }

        if (event.type == sf::Event::KeyPressed)
        {
            //painetaan esci�, l�hetet��n returnilla nollasta poikkeava luku, esim -1.
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                return -1;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
            {
                if (writeText && chatText.size() > 0)
                {
                    sendChatMSG((char *)players[0].name.c_str(), (char *)chatText.c_str());
                    char viesti[256] = {0};
                    sprintf(viesti, "MSG|%s|%s|", (char *)players[0].name.c_str(), (char *)chatText.c_str());
                    net.sendMsg(viesti);
                }
                writeText = !writeText;
                chatText = "";
                continue;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
            {
                if (net.isServer())
                {
                    Player *pl = &getNamelist()->at(0);
                    pl->ready = !pl->ready;

                    char buf[128] = {0};
                    if (pl->ready)
                        sprintf(buf, "READY|%s|%i|", (char *)players[0].name.c_str(), 1);
                    else
                        sprintf(buf, "READY|%s|%i|", (char *)players[0].name.c_str(), 0);

                    net.sendMsg(buf);
                }
                else
                {
                    char buf[128] = {0};
                    sprintf(buf, "READY|%s|%i|", (char *)players[0].name.c_str(), 1);
                    net.sendMsg(buf);
                }
            }

            if (net.isServer())
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
                {
                    getLvl()->rules.itemSpeed = !getLvl()->rules.itemSpeed;
                    sendRules();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
                {
                    getLvl()->rules.itemTimer = !getLvl()->rules.itemTimer;
                    sendRules();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
                {
                    getLvl()->rules.itemCooldown = !getLvl()->rules.itemCooldown;
                    sendRules();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
                {
                    getLvl()->rules.itemMover = !getLvl()->rules.itemMover;
                    sendRules();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
                {
                    getLvl()->rules.itemRange = !getLvl()->rules.itemRange;
                    sendRules();
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6))
                {
                    getLvl()->rules.los = !getLvl()->rules.los;
                    sendRules();
                }
            }
        }
    }

    if (writeText)
    {
        if (event.type == sf::Event::TextEntered)
        {
            if (event.text.unicode == 8)
            {
                if (chatText.size() > 0)
                    chatText.erase(chatText.begin() + chatText.size() - 1);
            }
            else
            {
                if (event.text.unicode != 13)
                {
                    if (event.text.unicode > 31)
                        chatText = chatText + static_cast<char>(event.text.unicode);
                }
            }
        }
    }

    sf::Vector2i pixelPos = sf::Mouse::getPosition(App);
    sf::Vector2f worldPos = mWindow.mapPixelToCoords(pixelPos);
    controls.mouseX = worldPos.x;
    controls.mouseY = worldPos.y;

    return 0;
}

void netThread()
{
    while (1)
    {
        mutex.lock();

        if (net.isConnected())
            net.listen();
        else
            break;

        if (stopNetThread)
            break;

        mutex.unlock();
        sf::sleep(sf::milliseconds(2));
    }

    mutex.unlock();
}

/*
    Lobby handler.
    Control whole lobby flow and start matches.
*/
void doLobby()
{
    sf::Thread netthread(&netThread);

    stopNetThread = false;
    float dt = 0.0; //deltalaskuri
    sf::Clock kello;
    kello.restart();
    float timeCounter = 0;

    gameStatus = G_MAINMENU;

    //if net is connected, we can check are we hosting or client.
    //we need ID before fo any futher.
    if (net.isConnected())
    {
        if (!net.isServer()) //we are client.
        {
            //wait server answer.
            //We want to know our ID. Server will send it to.
            while (1)
            {
                if (getMYID() == -1)
                {
                    /*
                        Listening any udp or tcp packets from server.
                        Server will send YOUR_ID message via TCP.
                        That's what we are looking for.
                    */
                    net.listen();
                }
                else
                {
                    //printf("myid: %i\n", getMYID());
                    if (!net.isServer())
                    {
                        net.sendMsg((char *)"SEND_SCORES|");
                    }
                    break;
                }
            }
        }
    }

    //finally we got ID from server if playing as client.
    //or setup server if playing as server.
    //se we can launch net code on thread.
    netthread.launch();


    while (1)
    {
        int res = lobbyInput(dt);
        if (res == -1)
            break;

        bool allReady = true;

        //game needs atleast 2 players.
        if (getNamelist()->size() < 2)
            allReady = false;

        //if even one is not ready, allready = false.
        for (unsigned int i = 0; i < getNamelist()->size(); i++)
        {
            Player *pl = &getNamelist()->at(i);
            if (!pl->ready)
                allReady = false;
        }

        //finally everybody is ready, so change game status.
        if (allReady)
            setGameStatus(G_INGAME);

        //if gamestatus = G_INGAME, start gameloop and after that
        //reset all ready to false.
        if (gameStatus == G_INGAME)
        {
            gameloop();
            gameStatus = G_MAINMENU;

            for (unsigned int i = 0; i < getNamelist()->size(); i++)
            {
                Player *pl = &getNamelist()->at(i);
                pl->ready = false;
            }
            kello.restart();
        }

        //we are not in match, so render lobby.
        renderLobby();

        dt = kello.getElapsedTime().asSeconds();
        kello.restart();
        timeCounter += dt;

        //if not server, send ping (keep connection alive)
        if (!net.isServer())
        {
            if (timeCounter >= 5.0)
            {
                net.sendPING();
                timeCounter -= 5.0;
            }
        }

        monitoring.deltatimer = dt;
        monitoring.fps = 1.0 / dt;
    }

    //disconnect from net and stop threads.
    //go back to lobby setup
    net.disconnect();
    stopNetThread = true;
    netthread.wait();
}

void sendHi()
{
    char buf[127] = {0};
    sprintf(buf, "HI|%s|", (char *)players[0].name.c_str());
    net.sendMsg(buf);
}

//try create a lobby as a host.
void createLobby()
{
    //clear namelist, because new one is coming.
    getNamelist()->clear();

    clearChatMSGs();
    sendChatMSG((char *)"game", (char *)"TEIT LOBBYN");

    loadingScreen();

    //generate random seed
    time_t ttime;
    time(&ttime);
    setSeed(ttime);

    //finally create server and run the lobby.
    //also, add host to first in namelist.
    if (net.createServer(sets->tcpPort) == 0)
    {
        addToNames(players[0].name);
        doLobby();
    }
    else
    {
        printf("Error. Can't create server.\n");
    }
}

//try join to lobby as a client.
void joinLobby()
{
    //clear namelist, because server will send the new one.
    getNamelist()->clear();
    clearChatMSGs();
    loadingScreen();

    //finally create client, send hi and run the lobby.
    if (net.createClient(sets->ipAddr, sets->tcpPort) == 0)
    {
        sendHi();
        doLobby();
    }
    else
    {
        printf("Error. Can't join.\n");
    }
}

void searchLobby(bool clientOrServer)
{
    gameStatus = G_MAINMENU;
    float dt = 0.0; //delta timer
    sf::Clock clock;
    clock.restart();
    float timeCounter = 0;

    while (1)
    {
        int res = searchInput(dt);
        if (res == -1)
            break;

        //render lobby setup to screen/window
        searchRender(clientOrServer);

        dt = clock.getElapsedTime().asSeconds();
        clock.restart();
        timeCounter += dt;

        if (timeCounter >= 1.0)
            timeCounter -= 1.0;

        monitoring.deltatimer = dt;
        monitoring.fps = 1.0 / dt;


        if (res == 1)
        {
            //name
            if (menuPos3 == 0)
            {
            }

            //ip
            if (menuPos3 == 1)
            {
            }
            //port
            if (menuPos3 == 2)
            {
            }
            
            //search
            if (menuPos3 == 3)
            {
                if (clientOrServer)
                    joinLobby();
                else
                    createLobby();
            }

            //poistu
            if (menuPos3 == 4)
            {
                break;
            }
        }
    }
}

void optionsRunning()
{
    gameStatus = G_MAINMENU;
    float dt = 0.0; //deltalaskuri
    sf::Clock clock;
    clock.restart();
    float timeCounter = 0;

    while (1)
    {
        int res = optionsInput(dt);
        if (res == -1)
            break;

        optionsRender();

        dt = clock.getElapsedTime().asSeconds();
        clock.restart();
        timeCounter += dt;

        if (timeCounter >= 1.0)
        {
            timeCounter -= 1.0;
        }

        monitoring.deltatimer = dt;
        monitoring.fps = 1.0 / dt;

        if (res == 3)
        {
            if (menuPos2 == 2)
            {
                sets->framelimit -= 1;
                if (sets->framelimit < 0)
                    sets->framelimit = 0;
            }

            if (menuPos2 == 4)
            {
                sets->masterVolume -= 0.01;
                if (sets->masterVolume < 0)
                    sets->masterVolume = 0;
            }
            if (menuPos2 == 5)
            {
                sets->musicVolume -= 0.01;
                if (sets->musicVolume < 0)
                    sets->musicVolume = 0;
            }
            if (menuPos2 == 6)
            {
                sets->soundVolume -= 0.01;
                if (sets->soundVolume < 0)
                    sets->soundVolume = 0;
            }

            if (menuPos2 == 7)
            {
                sets->fontSize -= 0.01;
                if (sets->fontSize < 0)
                    sets->fontSize = 0;
            }
            setGlobalVolumes(sets->masterVolume * 100.0);
        }
        if (res == 4)
        {
            if (menuPos2 == 2)
            {
                sets->framelimit += 1;
                if (sets->framelimit > 300)
                    sets->framelimit = 300;
            }

            if (menuPos2 == 4)
            {
                sets->masterVolume += 0.01;
                if (sets->masterVolume >= 1.0)
                    sets->masterVolume = 1.0;
            }

            if (menuPos2 == 5)
            {
                sets->musicVolume += 0.01;
                if (sets->musicVolume >= 1.0)
                    sets->musicVolume = 1.0;
            }

            if (menuPos2 == 6)
            {
                sets->soundVolume += 0.01;
                if (sets->soundVolume >= 1.0)
                    sets->soundVolume = 1.0;
            }

            if (menuPos2 == 7)
            {
                sets->fontSize += 0.01;
                if (sets->fontSize > 2.0)
                    sets->fontSize = 2.0;
            }

            setGlobalVolumes(sets->masterVolume * 100.0);
            setSoundVolumes(sets->soundVolume * 100.0);
        }

        if (res == 1 || res == 3 || res == 4)
        {
            //reso
            //TODO
            if (menuPos2 == 0)
            {
            }

            if (menuPos2 == 1)
            {
                sets->fullscreen = !sets->fullscreen;
            }

            if (menuPos2 == 3)
            {
                sets->vsync = !sets->vsync;
            }

            if (menuPos2 == 8)
            {
                initScreen();
                App.setFramerateLimit(sets->framelimit);
                setGlobalVolumes(sets->masterVolume * 100.0);
                setSoundVolumes(sets->soundVolume * 100.0);
                break;
            }
        }
    }
}

void mainMenuRunning()
{
    gameStatus = G_MAINMENU; //set right game status
    float dt = 0.0; //delta timer - timer between frames

    sf::Clock clock;
    clock.restart();
    float timeCounter = 0;

    while (1)
    {
        int res = menuInput(dt);
        if (res == -1) //quit
            break;

        menuRender();

        dt = clock.getElapsedTime().asSeconds();
        clock.restart();
        timeCounter += dt;

        if (timeCounter >= 1.0)
            timeCounter -= 1.0;

        monitoring.deltatimer = dt;
        monitoring.fps = 1.0 / dt;

        //want to execute something (check menuInput(dt))
        if (res == 1)
        {
            //menuPos 0 = create account
            //menuPos 1 = host
            //menuPos 2 = join
            //menuPos 3 = options
            //menuPos 4 = quit

            //go to lobby setup
            if (menuPos == 1 || menuPos == 2)
            {
                if (menuPos == 1)
                    searchLobby(false);
                else if (menuPos == 2)
                    searchLobby(true);

                dt = 0;
                clock.restart();
            }

            //options
            //go to options
            if (menuPos == 3)
            {
                menuPos2 = 0;
                optionsRunning();
                dt = 0;
                clock.restart();
            }

            //quit
            if (menuPos == 4)
                break;
        }
    }
    mutex.unlock();
}

std::string getMyNick()
{
    return players[0].name;
}

void soundThread()
{
    while (1)
    {
        mutex.lock();

        playSounds(cam.dx + ((sets->screenX / tilex) * 0.5), cam.dy + ((sets->screenY / tiley) * 0.5));

        if (stopThreads)
            break;

        mutex.unlock();
        sf::sleep(sf::milliseconds(2));
    }
}


int main()
{

    sf::Thread musicthread(&soundThread);

    time_t ttime;
    time(&ttime);
    srand(ttime);

    loadSettings();

    sets = &getSettings();
    setRules(0, 1, 1, 1, 1, 1);

    setGlobalVolumes(sets->masterVolume * 100.0);
    setSoundVolumes(sets->soundVolume * 100.0);

    players[0].name = sets->playername;

    //tell to engine what resource files we want to load.
    addTextureFile((const char *)"gfx/tilemap1.png");
    addTextureFile((const char *)"gfx/player.png");
    addTextureFile((const char *)"gfx/pommi.png");
    addTextureFile((const char *)"gfx/los.png");

    addFontFile((const char *)"fonts/arial.ttf");
    addFontFile((const char *)"fonts/expressway rg.ttf");
    addFontFile((const char *)"fonts/carbon bl.ttf");

    //initialise engine.
    //load resource files and set right settings
    if (init() == FAIL)
        return -1; 

    loadingScreen();

    loadSounds();

    musicthread.launch();

    mainMenuRunning();

    strcpy(sets->playername, (char *)players[0].name.c_str());

    //close engine
    if (quit() == FAIL)
        return -1; 
    stopThreads = true;
    musicthread.wait();
    return 0;
}

void setGameStatus(int st)
{
    gameStatus = st;
}

void setRules(bool los, bool cooldown, bool mover, bool range, bool speed, bool timer)
{
    getLvl()->rules.los = los;
    getLvl()->rules.itemCooldown = cooldown;
    getLvl()->rules.itemMover = mover;
    getLvl()->rules.itemRange = range;
    getLvl()->rules.itemSpeed = speed;
    getLvl()->rules.itemTimer = timer;
}

int getMYID()
{
    return net.getMYID();
}

void setUnitToDead(int id)
{
    Units[id].setAlive(false);
    addToSoundPool(SOUND_ID_death1, Units[id].getX(), Units[id].getY(), 1.0);
}

void changeItemDirection(int itemID, int lx, int ly)
{
    for (unsigned int i = 0; i < getLvl()->items.size(); i++)
    {
        if (getLvl()->items[i].itemID == itemID)
        {
            getLvl()->items[i].dx = lx;
            getLvl()->items[i].dy = ly;
            break;
        }
    }
}
