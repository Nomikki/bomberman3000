#include "resources.hpp"

std::vector<sf::Font> fontti;
std::vector<sf::Texture> texture;
std::vector<sf::Text> teksti;
std::vector<resType> res;

//load one specify image to texture
void loadImage(const char *name, int toID)
{
    //printf("Load image '%s' -> %i\n", name, toID);
    sf::Image img;

    if (img.loadFromFile(name))
    {
        img.createMaskFromColor(sf::Color(255, 0, 255));
        texture[toID].setSmooth(false);
        texture[toID].loadFromImage(img);
    }
    else
    {
        printf("failed to load '%s'\n", name);
    }
}

//load all images
void loadImgs()
{
    sf::Texture t;
    int a = 0;
    for (unsigned int i = 0; i < res.size(); i++)
    {
        if (res[i].ftype == 1)
        {
            texture.push_back(t);
            loadImage((const char *)res[i].filename.c_str(), a);
            a++;
        }
    }
}

//load one specify font.
void loadFont(const char *name, int toID)
{
    if (fontti[toID].loadFromFile(name))
    {
        teksti[toID].setFont(fontti[toID]);
        teksti[toID].setCharacterSize(16);
    }
    else
    {
        printf("Failed to load font '%s'\n", name);
    }
}

//load all fonts
void loadFonts()
{
    sf::Font f;
    sf::Text t;
    int a = 0;
    for (unsigned int i = 0; i < res.size(); i++)
    {
        if (res[i].ftype == 0)
        {
            teksti.push_back(t);
            fontti.push_back(f);
            loadFont((const char *)res[i].filename.c_str(), a);
            a++;
        }
    }
}

void clearImgs()
{
    texture.clear();
}

void clearFonts()
{
    teksti.clear();
    fontti.clear();
}

void addResourceFile(const char *name, int type)
{
    resType r;
    r.filename = name;
    r.ftype = type;
    res.push_back(r);
}

void addFontFile(const char *name)
{
    addResourceFile(name, 0);
}

void addTextureFile(const char *name)
{
    addResourceFile(name, 1);
}

sf::Texture &getTexture(int id)
{
    return texture[id];
}

sf::Font &getFont(int id)
{
    return fontti[id];
}

sf::Text &getTextfont(int id)
{
    return teksti[id];
}
