#ifndef SOUNDS_HPP_INCLUDED
#define SOUNDS_HPP_INCLUDED

#include "common.hpp"

#include <SFML/Audio.hpp>

//not the prettiest way to define sounds, but whatever.
#define SOUND_ID_MEDKIT 0
#define SOUND_ID_AMMO 1
#define SOUND_ID_ARMOR 2

#define SOUND_ID_drip1 3
#define SOUND_ID_drip2 4
#define SOUND_ID_drip3 5
#define SOUND_ID_drip4 6
#define SOUND_ID_drip5 7

#define SOUND_ID_rapidfire 8

#define SOUND_ID_hit1 9
#define SOUND_ID_hit2 10
#define SOUND_ID_hit3 11
#define SOUND_ID_hit4 12

#define SOUND_ID_explo01 13

#define SOUND_ID_oneshot01 14

#define SOUND_ID_reload 15

#define SOUND_ID_warmup 16
#define SOUND_ID_elect 17

#define SOUND_ID_walk01 18
#define SOUND_ID_walk02 19
#define SOUND_ID_walk03 20
#define SOUND_ID_walk04 21

#define SOUND_ID_MISSILE 22

#define SOUND_ID_death1 23
#define SOUND_ID_death2 24
#define SOUND_ID_death3 25
#define SOUND_ID_death4 26

/**
 * @brief Where that sound is playing (in level), with pitch and what sound (id)
 */
struct SoundpoolData
{
    float x;
    float y;
    float pitch;
    int id;
};

/**
 * @brief Load all sounds to buffers
 * Then setup listener and how it listen (imagine camera position and angles), 
 * so the "camera" is looking from top to down.
 */
void loadSounds();

/**
 * @brief Play sounds and update "camera" position,
 * so positional audio is working correctly.
 * This function is called in loop of soundthread (check main.cpp)
 * @param x 
 * @param y 
 */
void playSounds(float x, float y);

/**
 * @brief Play specific sound in position.
 * Its handled by playSounds(x, y) function.
 * 
 * @param id What kind of sound we want tok play
 * @param x  Position
 * @param y 
 * @param pitch 
 * @return int Return the channel where sound is assigned.
 */
int playSound(int id, float x, float y, float pitch);

/**
 * @brief stop sound by channel id.
 * When sound is added to bufferlist (ready to be played), it returns information what channel is will use.
 * We can use that information to stop that specific sound.
 * @param id 
 * @return int return 0.
 */
int stopSound(int id);

/**
 * @brief Stop all sounds.
 * 
 */
void stopAll();

/**
 * @brief Set the Global Volume
 * 
 * @param f 
 */
void setGlobalVolumes(float f);

/**
 * @brief Set the Sound Volume
 * 
 * @param f 
 */
void setSoundVolumes(float f);

/**
 * @brief If we want play sound in game, this is the function we are looking for.
 * It'll add information about that sound to pool.
 * And everytime we call playSounds(x, y), it check that pool and play sounds from it and
 * erase it from pool list.
 * 
 * @param id We need to know what sound we want to play
 * @param x  Position in level
 * @param y 
 * @param pitch 
 */
extern void addToSoundPool(int id, float x, float y, float pitch);

#endif // SOUNDS_HPP_INCLUDED
