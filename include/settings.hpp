#ifndef SETTINGS_HPP_INCLUDED
#define SETTINGS_HPP_INCLUDED

struct Settings
{
    int screenX, screenY; //window/screen resolution
    int optimize;         //WIP 0 = off, 1 = for-discard, 2 = quadtree
    bool fullscreen;      //0 = off, 1 = on.
    bool vsync;           //0 = off, 1 = on
    int framelimit;       //0 = uncapped fps

    float masterVolume; // 0.0 ... 1.0
    float musicVolume;  // 0.0 ... 1.0
    float soundVolume;  // 0.0 ... 1.0
    float fontSize;     // 0.0 ... 2.0, scaler

    char ipAddr[32];        //try remember last ip we connected
    uint32_t tcpPort;       //last tcp port se used
    uint32_t udpPortSEND;   //last udp port for send we used
    uint32_t udpPOrtLISTEN; //last udp port for listen we used

    uint32_t tickrate;  //how many times we communicate with server/client / second.
    uint8_t maxClients; //TODO
    bool compressed;    //TODO
    char playername[32];
};

/**
 * @brief Load settings from conf.cnf file and parse it.
 */
int loadSettings();

/**
 * @brief Save settings to conf.cnf.
 */
int saveSettings();

/**
 * @brief Create a default settings, like if settings not exists.
 * 
 */
void createDefaultSettings();

/**
 * @brief Get the Settings object
 * 
 * @return Settings* 
 */
Settings &getSettings();

#endif // SETTINGS_HPP_INCLUDED
