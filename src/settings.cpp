#include "common.hpp"
#include "settings.hpp"

Settings _settings;

int loadSettings()
{
    FILE *f = fopen("conf.cnf", "r");

    //file not founds, create default settings
    if (f == NULL)
    {
        createDefaultSettings();
        return 0;
    }

    //read line to buffer, so create buffer first
    char buf[127] = {0};

    //if not end of file, read to buffer and parse
    while (!feof(f))
    {
        fgets(buf, 127, f);
        int l = strlen(buf);
        if (l > 1)
        {
            buf[l - 1] = ' ';
            //printf("'buf: %s'\n", buf);
            std::vector<std::string> splittedBuf = explode(buf, " ");

            if (splittedBuf.size() == 3)
            {
                if (splittedBuf[0] == "screenX")
                    _settings.screenX = atoi((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "screenY")
                    _settings.screenY = atoi((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "fullscreen")
                    splittedBuf[2] == "1" ? _settings.fullscreen = true : _settings.fullscreen = false;

                if (splittedBuf[0] == "vsync")
                    splittedBuf[2] == "1" ? _settings.vsync = true : _settings.vsync = false;

                if (splittedBuf[0] == "framecap")
                    _settings.framelimit = atoi((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "masterVolume")
                    _settings.masterVolume = atof((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "musicVolume")
                    _settings.musicVolume = atof((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "soundVolume")
                    _settings.soundVolume = atof((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "fontSize")
                    _settings.fontSize = atof((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "ipAddr")
                    strcpy(_settings.ipAddr, (char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "tcpPort")
                    _settings.tcpPort = atoi((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "udpPortSEND")
                    _settings.udpPortSEND = atoi((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "udpPOrtLISTEN")
                    _settings.udpPOrtLISTEN = atoi((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "tickrate")
                    _settings.tickrate = atoi((char *)splittedBuf[2].c_str());

                if (splittedBuf[0] == "playername")
                    strcpy(_settings.playername, (char *)splittedBuf[2].c_str());
            }
        }
    }

    fclose(f);
    return 0;
}

int saveSettings()
{
    FILE *f = fopen("conf.cnf", "w");

    char buf[127] = {0};
    sprintf(buf, "screenX = %i\n", _settings.screenX);
    fputs(buf, f);

    sprintf(buf, "screenY = %i\n", _settings.screenY);
    fputs(buf, f);

    _settings.fullscreen ? sprintf(buf, "fullscreen = 1\n") : sprintf(buf, "fullscreen = 0\n");
    fputs(buf, f);

    sprintf(buf, "framecap = %i\n", _settings.framelimit);
    fputs(buf, f);

    _settings.vsync ? sprintf(buf, "vsync = 1\n") : sprintf(buf, "vsync = 0\n");
    fputs(buf, f);

    sprintf(buf, "masterVolume = %0.2f\n", _settings.masterVolume);
    fputs(buf, f);

    sprintf(buf, "musicVolume = %0.2f\n", _settings.musicVolume);
    fputs(buf, f);

    sprintf(buf, "soundVolume = %0.2f\n", _settings.soundVolume);
    fputs(buf, f);

    sprintf(buf, "fontSize = %0.2f\n", _settings.fontSize);
    fputs(buf, f);

    sprintf(buf, "ipAddr = %s\n", _settings.ipAddr);
    fputs(buf, f);

    sprintf(buf, "tcpPort = %u\n", _settings.tcpPort);
    fputs(buf, f);

    sprintf(buf, "udpPortSEND = %u\n", _settings.udpPortSEND);
    fputs(buf, f);

    sprintf(buf, "udpPOrtLISTEN = %u\n", _settings.udpPOrtLISTEN);
    fputs(buf, f);

    sprintf(buf, "tickrate = %u\n", _settings.tickrate);
    fputs(buf, f);

    sprintf(buf, "playername = %s\n", _settings.playername);
    fputs(buf, f);

    fclose(f);
    return 0;
}

void createDefaultSettings()
{
    _settings.screenX = 640;
    _settings.screenY = 480;
    _settings.optimize = 1;
    _settings.framelimit = 60;
    _settings.fullscreen = false;
    _settings.vsync = false;
    _settings.masterVolume = 100.0;
    _settings.musicVolume = 50.0f;
    _settings.soundVolume = 80.0;
    _settings.fontSize = 1.0;
    strcpy(_settings.ipAddr, "192.168.0.100");
    _settings.tcpPort = 7000;
    _settings.udpPOrtLISTEN = 1234;
    _settings.udpPortSEND = 1234;
    _settings.tickrate = 20;
    strcpy(_settings.playername, "unnamed hero!");
}

Settings &getSettings()
{
    return _settings;
}
