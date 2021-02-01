#ifndef RESOURCES_HPP_INCLUDED
#define RESOURCES_HPP_INCLUDED

#include "common.hpp"

/*
    Load and allocate resources like images, sounds, fonts etc
*/


struct resType
{
    std::string filename;
    int ftype;
};

//load one specify image to texture
/**
 * @brief 
 * 
 * @param name 
 * @param toID 
 */
extern void loadImage(const char *name, int toID);

//load all images
/**
 * @brief After preparing (addTextureFiles etc) you can call loadImgs.
 * 
 */
extern void loadImgs();

//load one specify font.
/**
 * @brief 
 * 
 * @param name resource name
 * @param toID array index, where to store it.
 */
extern void loadFont(const char *name, int toID);

//load all fonts
/**
 * @brief Load all fonts. Before that, you must call  
 * addFontFile(const char *name);
 * to tell to resource manager what fonts we want to load.
 * 
 */
extern void loadFonts();

/**
 * @brief Clear image list.
 * 
 */
extern void clearImgs();

/**
 * @brief Clear font list.
 * 
 */
extern void clearFonts();

/**
 * @brief Dont use directly. Its used by addFontFile or addTextureFile etc.
 * 
 * @param name filename.
 * @param type Resource type. 0 = font, 1 = texture
 */
extern void addResourceFile(const char *name, int type);

/**
 * @brief Add font file to resource list. Its preparing before load resources.
 * 
 * @param name filepath
 */
extern void addFontFile(const char *name);

/**
 * @brief Add texture file to resource list. Its preparing before load resources.
 * 
 * @param name filepath
 */
extern void addTextureFile(const char *name);

/**
 * @brief Get the reference of texture.
 * 
 * @param id which texture in texturelist.
 * @return sf::Texture& 
 */
extern sf::Texture &getTexture(int id);

/**
 * @brief Get the reference of font.
 * 
 * @param id which font in fontlist.
 * @return sf::Font& 
 */
extern sf::Font &getFont(int id);

/**
 * @brief Get the Textfont object
 * 
 * @param id 
 * @return sf::Text& 
 */
extern sf::Text &getTextfont(int id);

#endif // RESOURCES_HPP_INCLUDED
