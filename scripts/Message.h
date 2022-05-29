#pragma once
#include "Serializable.h"
#include  <string>
#include <SDL2/SDL.h>
#include "GOInfo.h"
class Player;

enum class MessageType: int8_t
{
    LOGIN   = 0,
    LOGOUT  = 1,
    NEWPLAYER = 2,
    PLAYERDEAD = 3,   
    PLAYERPOS = 4,
    DISPARO = 5,
    NEWBALA = 6,
    ESCUDO = 7,
    NEWESCUDO = 8
};

class Message: public Serializable{
protected:
    size_t messageSize = sizeof(MessageType);
    MessageType type;
    GOInfo goInfo;

public:
    
    Message(){};
    Message(MessageType type_ ,Player* player_);
    Message(MessageType type_ ,GOInfo* obj);
    
    virtual ~Message(){};

    virtual void to_bin();
    virtual int from_bin(char * bobj);

    size_t getMessageSize();
    MessageType getMessageType();
    GOInfo getGOInfo()const{ return goInfo; }
    void setGOInfo(const GOInfo& info){goInfo = info;}
    void setNjugador(int16_t n);    
    void setMsgType(MessageType type);
};