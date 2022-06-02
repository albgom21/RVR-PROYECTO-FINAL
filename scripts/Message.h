#pragma once
#include "Serializable.h"
#include  <string>
#include <SDL2/SDL.h>
#include "GOInfo.h"
class Player;

// Diferentes tipos de mensajes.
enum class MessageType : int8_t
{
	LOGIN = 0,
	LOGOUT = 1,

	NEWPLAYER = 2,
	PLAYERDEAD = 3,
	PLAYERPOS = 4,

	DISPARO = 5,
	NEWBALA = 6,
	BALAPOS = 7,
	BORRABALA = 8,

	ESCUDO = 9,
	NEWESCUDO = 10,
	BORRAESCUDO = 11
};

class Message : public Serializable {
protected:
	// Tamaño del mensaje. 
	size_t messageSize = sizeof(MessageType);
	// Tipo del mensaje.
	MessageType type;
	// Información del gameObject
	GOInfo goInfo;

public:
	// Diferentes constructoras.
	Message() {};
	Message(MessageType type_, Player* player_);
	Message(MessageType type_, GOInfo* obj);

	// Destructora,
	virtual ~Message() {};

	// Métodos de serialización.
	virtual void to_bin();
	virtual int from_bin(char* bobj);

	// Getters y Setters.
	size_t getMessageSize();
	MessageType getMessageType();
	GOInfo getGOInfo()const { return goInfo; }
	void setGOInfo(const GOInfo& info) { goInfo = info; }
	void setNjugador(int16_t n);
	void setMsgType(MessageType type);
};