#include "Message.h"
#include <memory.h>
#include "Player.h"
#include <iostream>


Message::Message(MessageType type_, Player *player_) : type(type_)
{   
    goInfo = GOInfo();
    goInfo.nJug = player_->getNum();
    goInfo.pos = player_->getPos();
}

size_t Message::getMessageSize()
{
    return messageSize;
}

MessageType Message::getMessageType()
{
    return type;
}

void Message::to_bin()
{
    messageSize = sizeof(MessageType) + sizeof(GOInfo);

    alloc_data(messageSize);

    memset(_data, 0, messageSize);

    char *temp = _data;

    memcpy(temp, &type, sizeof(MessageType));

    temp += sizeof(MessageType);
  
    memcpy(temp, &goInfo, sizeof(GOInfo));
}

int Message::from_bin(char *bobj)
{

    messageSize = sizeof(MessageType);
    alloc_data(messageSize);
    memcpy(static_cast<void *>(_data), bobj, messageSize);

    char *temp = _data;

    memcpy(&type, temp, sizeof(MessageType));    

    messageSize = sizeof(MessageType) + sizeof(GOInfo);
    alloc_data(messageSize);
    memcpy(static_cast<void *>(_data), bobj, messageSize);
    temp = _data;
    temp += sizeof(MessageType);
    
    memcpy(&goInfo, temp, sizeof(GOInfo));
    return 0;
}


void Message::setMsgType(MessageType type_)
{
    type = type_;
}