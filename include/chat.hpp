#ifndef CHAT_HPP_INCLUDED
#define CHAT_HPP_INCLUDED

#include "common.hpp"

/**
 * @brief Chat message contains 2 informations: who is sender and the message.
 * 
 */
struct ChatMSG
{
   std::string sender;
   std::string msg;
};

/**
 * @brief Chat container. All chat messages is added here.
 * 
 */
struct Chat
{
   std::vector<ChatMSG> msgs;
};

/**
 * @brief Get the Chat object
 * 
 * @return Chat* 
 */
extern Chat *getChat();

/**
 * @brief Send message to server
 * 
 * @param sender Player nickname
 * @param msg    Message
 */
void sendChatMSG(const char *sender, const char *msg);

/**
 * @brief Just add text, but dont send it
 * 
 * @param sender 
 * @param msg 
 */
void addChatText(const char *sender, const char *msg);

/**
 * @brief Clear whole chat
 * 
 */
void clearChatMSGs();



#endif // CHAT_HPP_INCLUDED
