#ifndef NETWORK_HPP_INCLUDED
#define NETWORK_HPP_INCLUDED

#include <cmath>
#include <cstdio> //cstdio mahdollistaa sen, että voidaan tulostaa suoraan konsolinäkymään jotain tekstiä.
#include <vector>
#include <string.h>
#include <list>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <enet/enet.h>

#include "common.hpp"

#define MAX_PLAYERS 4

class Network
{
    bool iAmServer;
    bool connected;

    //TCP
    sf::TcpSocket socket;        //for client
    sf::TcpListener listener;    //for server
    sf::SocketSelector selector; //for server

    std::list<sf::TcpSocket *> clients;

    //UDP
    ENetAddress address; //for server
    ENetHost *UDPserver; //for server
    ENetHost *UDPclient; //for client
    ENetPeer *peer;

    //all free IDs
    std::vector<int> id_list;

    //hold our ID
    int myID;

    float pingCounter;
    sf::Clock pingClock;

public:
    Network();
    ~Network();

    /**
     * @brief Create a Server object
     * 
     * @param port 
     * @return int 
     */
    int createServer(uint32_t port);

    /**
     * @brief Create a Client object
     * 
     * @param addr 
     * @param port 
     * @return int 
     */
    int createClient(char *addr, uint32_t port);

    /**
     * @brief 
     * 
     */
    void listen();

    /**
     * @brief 
     * 
     */
    void disconnect();

    /**
     * @brief 
     * 
     * @param msg 
     */
    void sendMsg(char *msg);

    /**
     * @brief 
     * 
     * @param msg 
     */
    void sendUDP(char *msg);

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool isServer();

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool isConnected();

    /**
     * @brief 
     * 
     * @param msg 
     * @return int 
     */
    int sendToAll(char *msg);

    /**
     * @brief 
     * 
     * @param client 
     * @param msg 
     */
    void sendTo(sf::TcpSocket &client, char *msg);

    /**
     * @brief 
     * 
     * @return int 
     */
    int getMYID();

    /**
     * @brief 
     * 
     */
    void sendPING();

    /**
     * @brief Get the Ping object
     * 
     * @return float 
     */
    float getPing();
};

#endif // NETWORK_HPP_INCLUDED
