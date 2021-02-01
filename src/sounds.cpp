#include "sounds.hpp"
#include <cstdio>
#include <SFML/Audio/Sound.hpp>

std::vector<SoundpoolData> soundpool;

sf::Mutex mut;
sf::SoundBuffer buffer[32];
sf::Sound sound[64];
int soundCounter;
int soundCounterLoop;
float soundVolume;

void loadSounds()
{
    soundCounter = 0;
    soundCounterLoop = 32;
    soundpool.clear();

    if (!buffer[0].loadFromFile("sfx/medkit_pickup.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[0].setBuffer(buffer[0]);

    if (!buffer[1].loadFromFile("sfx/ammo_pickup.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[1].setBuffer(buffer[1]);

    if (!buffer[2].loadFromFile("sfx/armor_pickup.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[2].setBuffer(buffer[2]);

    //Dripit
    if (!buffer[3].loadFromFile("sfx/drip01.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[3].setBuffer(buffer[3]);

    if (!buffer[4].loadFromFile("sfx/drip02.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[4].setBuffer(buffer[4]);

    if (!buffer[5].loadFromFile("sfx/drip03.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[5].setBuffer(buffer[5]);

    if (!buffer[6].loadFromFile("sfx/drip04.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[6].setBuffer(buffer[6]);

    if (!buffer[7].loadFromFile("sfx/drip05.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[7].setBuffer(buffer[7]);

    if (!buffer[8].loadFromFile("sfx/rapidfire01.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[8].setBuffer(buffer[8]);

    if (!buffer[9].loadFromFile("sfx/hit01.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[9].setBuffer(buffer[9]);

    if (!buffer[10].loadFromFile("sfx/hit02.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[10].setBuffer(buffer[10]);

    if (!buffer[11].loadFromFile("sfx/hit03.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[11].setBuffer(buffer[11]);

    if (!buffer[12].loadFromFile("sfx/hit04.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[12].setBuffer(buffer[12]);

    if (!buffer[13].loadFromFile("sfx/explo01.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[13].setBuffer(buffer[13]);

    if (!buffer[14].loadFromFile("sfx/launcher_fire02.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[14].setBuffer(buffer[14]);

    if (!buffer[15].loadFromFile("sfx/reload.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[15].setBuffer(buffer[15]);

    if (!buffer[16].loadFromFile("sfx/warmup.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[16].setBuffer(buffer[16]);

    if (!buffer[17].loadFromFile("sfx/elec.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[17].setBuffer(buffer[17]);

    if (!buffer[18].loadFromFile("sfx/step01.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[18].setBuffer(buffer[18]);

    if (!buffer[19].loadFromFile("sfx/step02.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[19].setBuffer(buffer[19]);

    if (!buffer[20].loadFromFile("sfx/step03.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[20].setBuffer(buffer[20]);

    if (!buffer[21].loadFromFile("sfx/step04.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[21].setBuffer(buffer[21]);

    if (!buffer[22].loadFromFile("sfx/missile.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[22].setBuffer(buffer[22]);

    if (!buffer[23].loadFromFile("sfx/d1.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[23].setBuffer(buffer[23]);

    if (!buffer[24].loadFromFile("sfx/d2.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[24].setBuffer(buffer[24]);

    if (!buffer[25].loadFromFile("sfx/d3.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[25].setBuffer(buffer[25]);

    if (!buffer[26].loadFromFile("sfx/d4.wav"))
    {
        printf("Sound file not found.\n");
        return;
    }
    sound[26].setBuffer(buffer[26]);

    soundCounter = 0;
    soundCounterLoop = 32;

    sf::Listener::setDirection(0.f, 0.f, 1.0f);
    sf::Listener::setUpVector(1.f, -1.f, 0.f);
}

void setGlobalVolumes(float f)
{
    sf::Listener::setGlobalVolume(f);
}

void setSoundVolumes(float f)
{
    soundVolume = f;
}

int playSound(int id, float x, float y, float pitch)
{
    int soundCounter_back = 0;
    if (id >= 0)
    {
        sound[soundCounter].setBuffer(buffer[id]);

        if (id == SOUND_ID_walk01 ||
            id == SOUND_ID_walk02 ||
            id == SOUND_ID_walk03 ||
            id == SOUND_ID_walk04)
        {
            sound[soundCounter].setMinDistance(5.f);
            sound[soundCounter].setAttenuation(10.0f);
        }
        else
        {
            sound[soundCounter].setMinDistance(5.f);
            sound[soundCounter].setAttenuation(10.0f);
        }

        sound[soundCounter].setVolume(soundVolume);
        sound[soundCounter].setLoop(false);
        sound[soundCounter].setPitch(pitch);
        sound[soundCounter].setPosition(x, y, 0);

        sound[soundCounter].play();
        soundCounter_back = soundCounter;
        soundCounter++;

        if (soundCounter > 63)
            soundCounter = 0;
    }

    return soundCounter_back;
}

int stopSound(int id)
{
    sound[id].stop();
    return 0;
}

void playSounds(float x, float y)
{
    mut.lock();
    while (1)
    {
        if (soundpool.size() > 0)
        {
            //printf(".");
            playSound(soundpool[0].id, soundpool[0].x, soundpool[0].y, soundpool[0].pitch);
            soundpool.erase(soundpool.begin() + 0);
        }
        else
        {
            break;
        }
    }

    sf::Listener::setPosition(x, y, 2);
    sf::sleep(sf::milliseconds(2));
    mut.unlock();
}

void addToSoundPool(int id, float x, float y, float pitch)
{
    mut.lock();
    SoundpoolData p;
    p.id = id;
    p.x = x;
    p.y = y;
    p.pitch = pitch;
    soundpool.push_back(p);
    mut.unlock();
}

void stopAll()
{
    for (int i = 0; i < 64; i++)
    {
        sound[i].pause();
        sound[i].stop();
    }
}
