#include "Network.hpp"

sf::Mutex netmute;

Network::Network()
{
    pingClock.restart();
    pingCounter = 0;
}

Network::~Network()
{
}

void Network::sendPING()
{
    pingCounter = 0;
    pingClock.restart();

    sendMsg((char *)"PING|0|");
}

float Network::getPing()
{
    return pingCounter;
}

int Network::createServer(uint32_t port)
{
    printf("Create server, port: %u.\n", port);
    connected = false;

    selector.clear();
    clients.clear();

    // bind the listener to a port
    if (listener.listen(port) != sf::Socket::Done)
        return -1;

    // bind the socket to a port
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    /*  Bind the server to the default localhost.
        A specific host address can be specified by
        enet_address_set_host (& address, "x.x.x.x");
    */
    address.host = ENET_HOST_ANY;

    // Bind the server to port 1234.
    address.port = 1234;
    UDPserver = enet_host_create(&address /* the address to bind the server host to */,
                                 32 /* allow up to 32 clients and/or outgoing connections */,
                                 2 /* allow up to 2 channels to be used, 0 and 1 */,
                                 0 /* assume any amount of incoming bandwidth */,
                                 0 /* assume any amount of outgoing bandwidth */);
    if (UDPserver == NULL)
    {
        fprintf(stderr,
                "An error occurred while trying to create an ENet server host.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("UDP server created.\n");
    }

    selector.add(listener);
    iAmServer = true;
    connected = true;

    //Build id-list:
    id_list.clear();

    //start from 1, because 0 is reserved for host.
    for (int i = 1; i < MAX_PLAYERS; i++)
    {
        id_list.push_back(i);
    }
    myID = 0;

    return 0;
}

int Network::createClient(char *addr, uint32_t port)
{
    connected = false;

    myID = -1;
    id_list.clear();

    printf("Create client to %s:%u.\n", addr, port);

    sf::Socket::Status status = socket.connect(addr, port);
    if (status != sf::Socket::Done)
    {
        printf("tcp error.\n");
        return -1;
    }

    socket.setBlocking(false);

    // bind the socket to a port
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    UDPclient = enet_host_create(NULL /* create a client host */,
                                 1 /* only allow 1 outgoing connection */,
                                 2 /* allow up 2 channels to be used, 0 and 1 */,
                                 576000 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                                 144000 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
    if (UDPclient == NULL)
    {
        fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("UDP client created.\n");
    }

    // Connect to some.server.net:1234.
    enet_address_set_host(&address, addr);
    address.port = 1234;
    // Initiate the connection, allocating the two channels 0 and 1.
    peer = enet_host_connect(UDPclient, &address, 2, 0);
    if (peer == NULL)
    {
        fprintf(stderr, "No available peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }
    // Wait up to 5 seconds for the connection attempt to succeed.
    ENetEvent event;
    if (enet_host_service(UDPclient, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("Connection to some.server.net:1234 succeeded.\n");
        connected = true;
    }
    else
    {
        /*
        Either the 5 seconds are up or a disconnect event was received.
        Reset the peer in the event the 5 seconds had run out without any significant event.
        */
        enet_peer_reset(peer);
        printf("Connection to some.server.net:1234 failed.\n");
        return -1;
    }

    iAmServer = false;
    connected = true;
    return 0;
}

int Network::sendToAll(char *msg)
{
    for (std::list<sf::TcpSocket *>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        sf::TcpSocket &client = **it;
        sf::Packet packet;
        packet << msg;

        if (client.send(packet) != sf::Socket::Done)
        {
            return 1;
        }
    }
    return 0;
}

void Network::sendTo(sf::TcpSocket &client, char *msg)
{
    sf::Packet packet;
    packet << msg;

    if (client.send(packet) != sf::Socket::Done)
    {
        printf("Error. Didn't sent '%s' to Client.\n", msg);
    }
}

void Network::listen()
{
    netmute.lock();

    if (isServer())
    {
        ENetEvent event;
        bool sendMore = false;
        char da[1024] = {0};
        while (enet_host_service(UDPserver, &event, 1) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                //printf ("A new client connected from %x:%u.\n", event.peer->address.host,   event.peer->address.port);
                //event.peer -> data = "Client information";
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                /*
                printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                        event.packet->dataLength,
                        event.packet->data,
                        event.peer->data,
                        event.channelID);
                        */
                parseNetPacket((const char *)event.packet->data);
                sendMore = true;
                strcpy(da, (char *)event.packet->data);
                //enet_host_flush (UDPserver);
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                //printf ("%s disconnected.\n", event.peer->data);
                event.peer->data = NULL;
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
            }
        }
        if (sendMore)
        {
            sendUDP(da);
        }
    }
    else
    {
        ENetEvent event;
        while (enet_host_service(UDPclient, &event, 1) > 0)
        {
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                //printf ("A new client connected from %x:%u.\n", event.peer->address.host,   event.peer->address.port);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                parseNetPacket((char *)event.packet->data);
                enet_packet_destroy(event.packet);

                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                //printf ("%s disconnected.\n", event.peer->data);
                event.peer->data = NULL;
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
            }
        }
    }

    if (!isServer())
    {
        sf::Packet packet;
        char data[127] = {0};

        if (socket.receive(packet) == sf::Socket::Done)
        {
            packet >> data;
            parseNetPacket(data);
            std::vector<std::string> splittedBuf = explode(data, "|");
            if (splittedBuf[0] == "YOUR_ID")
            {
                myID = atoi((char *)splittedBuf[1].c_str());
                setSeed(atoi((char *)splittedBuf[2].c_str()));
            }
            if (splittedBuf[0] == "PONG")
            {
                pingCounter = pingClock.getElapsedTime().asSeconds();
                pingClock.restart();
            }
        }

        sf::sleep(sf::milliseconds(2));
        netmute.unlock();
        return;
    }

    if (selector.wait(sf::seconds(1.0 / 10000.0)))
    {
        if (selector.isReady(listener))
        {
            if (clients.size() < MAX_PLAYERS)
            {
                sf::TcpSocket *_client = new sf::TcpSocket;
                if (listener.accept(*_client) == sf::Socket::Done)
                {
                    _client->setBlocking(false);
                    // Add the new client to the clients list
                    clients.push_back(_client);

                    // Add the new client to the selector so that we will
                    // be notified when he sends something

                    selector.add(*_client);
                    //printf("New client joined!\n");

                    //the point is find lowest free id and set it to player/client.
                    int lowest_id_position = id_list.size(); //init with size of list
                    int lowest = 4;

                    for (unsigned int o = 0; o < id_list.size(); o++)
                    {
                        if (id_list[o] < lowest)
                        {
                            lowest = id_list[o];
                            lowest_id_position = o;
                        }
                    }

                    sf::Packet p;
                    char dd[32] = {0};
                    sprintf(dd, "YOUR_ID|%i|%i|", id_list[lowest_id_position], getSeed());
                    std::string testi = dd;
                    p << testi;
                    _client->send(p);

                    //id is reserved, so remove it from id_list
                    id_list.erase(id_list.begin() + lowest_id_position);
                }
                else
                {
                    // Error, we won't get a new connection, delete the socket
                    delete _client;
                }
            }
            else
            {
                printf("Too many connections.\n");
            }
        }
        else
        {
            // The listener socket is not ready, test all other sockets (the clients)
            for (std::list<sf::TcpSocket *>::iterator it = clients.begin(); it != clients.end(); ++it)
            {
                sf::TcpSocket &client = **it;
                if (&client != nullptr && selector.isReady(client))
                {
                    // The client has sent some data, we can receive it
                    sf::Packet packet;
                    char data[127] = {0};
                    sf::Socket::Status status = client.receive(packet);

                    if (status == sf::Socket::Done)
                    {
                        packet >> data;
                        printf("client sent: '%s'", data);
                        parseNetPacket(data);
                        std::vector<std::string> splittedBuf = explode(data, "|");
                        if (splittedBuf[0] == "MSG")
                        {
                            sendToAll(data);
                        }

                        if (splittedBuf[0] == "POS")
                        {
                            sendToAll(data);
                        }

                        if (splittedBuf[0] == "BOMB")
                        {
                            sendToAll(data);
                        }

                        if (splittedBuf[0] == "PING")
                        {
                            sprintf(data, "PONG|%s|", (char *)splittedBuf[1].c_str());
                            sendTo(client, data);
                        }

                        if (splittedBuf[0] == "DEAD")
                        {
                            parseNetPacket(data);
                            sendToAll(data);
                        }

                        if (splittedBuf[0] == "READY")
                        {
                            parseNetPacket(data);
                            sendToAll(data);
                        }

                        //remove id, so put that id back to id_list
                        if (splittedBuf[0] == "REMID")
                        {
                            int _id = atoi((char *)splittedBuf[1].c_str());
                            id_list.push_back(_id);

                            remFromNames((char *)splittedBuf[2].c_str());
                            sendToAll((char *)"CLEAR_NAMES|");
                            sprintf(data, (char *)"DC|%s|", (char *)splittedBuf[2].c_str());
                            sendToAll(data);
                        }

                        if (splittedBuf[0] == "ITEMDIR")
                        {
                            parseNetPacket(data);
                            sendToAll(data);
                        }

                        if (splittedBuf[0] == "ADDITEM")
                            sendToAll(data);

                        if (splittedBuf[0] == "HI")
                        {
                            sendToAll(data);

                            //send namelist back to client
                            for (unsigned int k = 0; k < getNamelist()->size(); k++)
                            {
                                Player &pl = getNamelist()->at(k);
                                int re = 0;
                                if (pl.ready)
                                    re = 1;
                                sprintf(data, (char *)"NAME|%s|%i|", (char *)pl.name.c_str(), re);
                                sendTo(client, data);
                            }

                            sendRules();
                        }
                    }
                    if (status == sf::Socket::Disconnected)
                    {
                        printf("Client disconnected\n");
                        selector.remove(client);
                        client.disconnect();
                        delete (&client);

                        clients.erase(it);
                        it--;
                        break;
                    }
                }
            }
        }
    }

    sf::sleep(sf::milliseconds(2));
    netmute.unlock();
}

void Network::disconnect()
{
    printf("disconnect\n");

    if (!isServer())
    {
        char da[32] = {0};
        sprintf(da, "REMID|%i|%s|", getMYID(), (char *)getMyNick().c_str());
        sendMsg(da);
    }

    listener.close();
    socket.disconnect();

    for (std::list<sf::TcpSocket *>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (*it != nullptr)
        {
            sf::TcpSocket &client = **it;

            selector.remove(client);
            client.disconnect();
            delete (&client);
            clients.erase(it);
            it--;
        }
    }

    if (UDPserver != NULL)
        enet_host_destroy(UDPserver);
    if (UDPclient != NULL)
        enet_host_destroy(UDPclient);

    connected = false;
}

void Network::sendUDP(char *msg)
{
    /* Create a reliable packet of size 7 containing "packet\0" */
    if (!isServer())
    {
        ENetPacket *packet = enet_packet_create(msg, strlen(msg) + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
        /* One could just use enet_host_service() instead. */
        enet_host_flush(UDPclient);
    }
    else
    {

        ENetPacket *packet = enet_packet_create(msg, strlen(msg) + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(UDPserver, 0, packet);
        enet_host_flush(UDPserver);
    }
}

void Network::sendMsg(char *msg)
{
    if (!isConnected())
        return;

    if (isServer())
    {
        printf("sent: %s\n", msg);
        sendToAll(msg);
        return;
    }

    printf("send msg: %s\n", msg);
    sf::Packet packet;
    packet << msg;
    if (socket.send(packet) != sf::Socket::Done)
    {
        printf("Something went wrong.\n");
    }
}

bool Network::isServer()
{
    return iAmServer;
}

bool Network::isConnected()
{
    return connected;
}

int Network::getMYID()
{
    return myID;
}
