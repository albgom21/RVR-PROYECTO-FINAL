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


Message::Message(MessageType type_, GOInfo *obj) : type(type_)
{   
    goInfo = GOInfo();
    goInfo.nJug = obj->nJug;
    goInfo.pos = obj->pos;
    goInfo.id = obj->id;
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
    switch (type)
    {
    case MessageType::LOGIN:  case MessageType::LOGOUT: 
        {
            messageSize = sizeof(MessageType) ;
            alloc_data(messageSize);
            memset(_data, 0, messageSize);
            char *temp = _data;
            memcpy(temp, &type, sizeof(MessageType));}
        break;
    
    default:{
        messageSize = sizeof(MessageType) + sizeof(GOInfo);

    alloc_data(messageSize);

    memset(_data, 0, messageSize);

    char *temp = _data;

    memcpy(temp, &type, sizeof(MessageType));

    temp += sizeof(MessageType);
  
    memcpy(temp, &goInfo, sizeof(GOInfo));
    }
        break;
    }
    
};

int Message::from_bin(char *bobj) {

    //reservamos memoria para coger el tipo de mensaje
    messageSize = sizeof(MessageType);
    alloc_data(messageSize);
    //coger el tipo de mensaje
    memcpy(static_cast<void *>(_data), bobj, messageSize);

    //Reconstruir la clase usando el buffer _data
    char *temp = _data;

    //Copiamos tipo
    memcpy(&type, temp, sizeof(MessageType));

    switch (type)
    {
    case MessageType::LOGIN: case MessageType::LOGOUT: 
        {
            messageSize = sizeof(MessageType);
            alloc_data(messageSize);
            memcpy(static_cast<void *>(_data), bobj, messageSize);
            char *temp = _data;
            temp += sizeof(MessageType);
        }
        break;
    
    default:{
          messageSize = sizeof(MessageType) + sizeof(GOInfo);
    alloc_data(messageSize);
    memcpy(static_cast<void *>(_data), bobj, messageSize);
    temp = _data;
    temp += sizeof(MessageType);
    
    memcpy(&goInfo, temp, sizeof(GOInfo));
    }
        break;
    }



    // messageSize = sizeof(MessageType);
    // alloc_data(messageSize);
    // memcpy(static_cast<void *>(_data), bobj, messageSize);

    // char *temp = _data;

    // memcpy(&type, temp, sizeof(MessageType));    

    // messageSize = sizeof(MessageType) + sizeof(GOInfo);
    // alloc_data(messageSize);
    // memcpy(static_cast<void *>(_data), bobj, messageSize);
    // temp = _data;
    // temp += sizeof(MessageType);
    
    // memcpy(&goInfo, temp, sizeof(GOInfo));
    return 0;
}


void Message::setMsgType(MessageType type_)
{
    type = type_;
}