#include "chat.hpp"

Chat _chat;

void sendChatMSG(const char *sender, const char *msg)
{
    addChatText((char *)"ME", msg);
}

void clearChatMSGs()
{
    _chat.msgs.clear();
}

void addChatText(const char *sender, const char *msg)
{
    ChatMSG m;
    m.msg = msg;
    m.sender = sender;
    _chat.msgs.push_back(m);
}


Chat *getChat()
{
    return &_chat;
}
