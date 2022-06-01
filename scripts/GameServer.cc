#include "GameServer.h"
#include "Message.h"
#include <memory>
#include <ctime>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <list>
#include "Constants.h"


GameServer::GameServer(const char* s, const char* p) : socket(s, p)
{
	initTime = SDL_GetTicks();
}

void GameServer::do_messages()
{
	if (socket.bind() == -1)
		std::cout << "ERROR: en Bind\n";

	while (true)
	{
		Message cm;
		Socket* s = nullptr;

		if (socket.recv(cm, s) == -1)
			std::cout << "ERROR: no se pudo recibir el mensaje\n";

		switch (cm.getMessageType())
		{
		case MessageType::LOGIN:
		{
			clients.push_back(std::move(std::make_unique<Socket>(*s)));
			GOInfo n;
			n.nJug = nPlayers;
			std::cout << "NPLAYERS: " << n.nJug << "\n";

			if (nPlayers == 0)
				n.pos = Vector2D(X_INI, Y_INI);
			else
				n.pos = Vector2D(X_INI_2, Y_INI);

			players.push_back(n);

			Message newPlayerConnected = Message();
			newPlayerConnected.setMsgType(MessageType::NEWPLAYER);
			newPlayerConnected.setGOInfo(players[nPlayers]);

			//VER SI SE PUEDE REFACTORIZAR LOS FORs YA QUE SOLO HAY A LO SUMO 2 CLIENTES

			for (auto it = clients.begin(); it != clients.end(); it++) {
				if (*((*it).get()) != *s)
					if (socket.send(newPlayerConnected, *(*it)) == -1)  std::cout << "ERROR: 0\n";;
			}

			for (auto it = players.begin(); it != players.end(); ++it)
			{
				if ((*it).nJug != cm.getGOInfo().nJug)
				{
					newPlayerConnected.setGOInfo(*it);
					if (socket.send(newPlayerConnected, *s) == -1)  std::cout << "ERROR: 1\n";;
				}
			}

			nPlayers++;
			break;
		}

		case MessageType::LOGOUT:
		{

			auto it = clients.begin();

			while (it != clients.end() && (**it != *s))
				++it;

			if (it == clients.end())
				std::cout << "El usuario no se encontraba conectado\n";
			else
			{
				std::cout << "Usuario desconectado" << "\n";
				clients.erase(it);
			}
			if (clients.size() == 0)
				fin = true;

			break;
		}

		case MessageType::PLAYERPOS:
		{
			players[cm.getGOInfo().nJug] = cm.getGOInfo();

			for (auto it = clients.begin(); it != clients.end(); it++)
			{
				if (*((*it).get()) != *s)
					if (socket.send(cm, (*((*it).get()))) == -1)  std::cout << "ERROR: 2\n";;
			}

			break;
		}

		case MessageType::ESCUDO:
		{
			GOInfo obj;
			int offset = 110;
			if (cm.getGOInfo().nJug == 1) offset = -(-TAM_JUG_X + 110 + TAM_SHIELD_X);



			obj.pos = Vector2D(cm.getGOInfo().pos.getX() + offset, cm.getGOInfo().pos.getY() + (TAM_JUG_Y / 2) - (TAM_SHIELD_Y / 2));
			obj.nJug = cm.getGOInfo().nJug;
			obj.id = nShields;

			bool correctPos = true;
			//Ver que al crear un escudo no colisione con otro
			for (auto s : shields) {
				SDL_Rect a, b;
				GOInfo newShield = obj;
				GOInfo shi = s.second;
				a = { (int)obj.pos.getX(), (int)obj.pos.getY(), TAM_SHIELD_X, TAM_SHIELD_Y };
				b = { (int)shi.pos.getX(), (int)shi.pos.getY(), TAM_SHIELD_X, TAM_SHIELD_Y };

				if (SDL_HasIntersection(&a, &b)) {
					std::cout << "NO SE PUEDE CREAR ESCUDO\n";
					correctPos = false;
				}
			}
			if (correctPos) {
				shields[nShields] = obj;

				nShields++;

				Message cm = Message(MessageType::NEWESCUDO, &obj);
				for (auto i = clients.begin(); i != clients.end(); ++i)
					if (socket.send(cm, (*(*i))) == -1) std::cout << "ERROR: 3\n";
			}


			break;
		}

		case MessageType::DISPARO:
		{
			GOInfo* obj = new GOInfo();
			int offset = 50;
			if (cm.getGOInfo().nJug == 1)  offset = -(-TAM_JUG_X + 50 + TAM_BULLET_X);

			obj->pos = Vector2D(cm.getGOInfo().pos.getX() + offset, cm.getGOInfo().pos.getY() + (TAM_JUG_Y / 2) - (TAM_BULLET_Y / 2));
			obj->nJug = cm.getGOInfo().nJug;
			obj->id = nBullets;
			bullets[nBullets] = obj;
			Message cm = Message(MessageType::NEWBALA, obj);
			nBullets++;
			for (auto i = clients.begin(); i != clients.end(); ++i)
				if (socket.send(cm, (*(*i))) == -1)  std::cout << "ERROR: 4\n";;

			break;
		}
		default:
			break;
		}

		if (cm.getMessageType() != MessageType::LOGIN)
			delete s;

	}
}

void GameServer::move_bullets() {

	if (SDL_GetTicks() - initTime > timeUpdate)
	{
		std::list<GOInfo*> bulletsDelete;
		double_t x = 0;
		for (auto b : bullets) {
			b.second->nJug == 1 ? x = -VELOCITY : x = VELOCITY;

			if (x < 0 && b.second->pos.getX() < -TAM_BULLET_X)
				bulletsDelete.push_back(b.second);
			else if (x > 0 && b.second->pos.getX() > W_WIDTH)
				bulletsDelete.push_back(b.second);
			else {
				b.second->pos.setX(b.second->pos.getX() + x);
				Message msg = Message(MessageType::BALAPOS, b.second);
				for (auto it = clients.begin(); it != clients.end(); ++it)
					if (socket.send(msg, *(*it)) == -1)  std::cout << "ERROR: 5\n";;
			}
		}
		for (auto o = bulletsDelete.begin(); o != bulletsDelete.end(); ++o) {
			Message msg = Message(MessageType::BORRABALA, *o);

			for (auto it = clients.begin(); it != clients.end(); ++it)
				if (socket.send(msg, *(*it)) == -1)  std::cout << "ERROR: 6\n";;

			bullets.erase((*o)->id);
		}

		initTime = SDL_GetTicks();
	}
}

void GameServer::collisions()
{
	std::list<GOInfo*> bulletsDelete;
	std::list<GOInfo> shieldsDelete;

	for (auto it = bullets.begin(); it != bullets.end(); ++it)
	{
		for (auto it2 = shields.begin(); it2 != shields.end(); ++it2)
		{
			SDL_Rect a, b;
			GOInfo* bul = (*it).second;
			GOInfo shi = (*it2).second;
			a = { (int)bul->pos.getX(), (int)bul->pos.getY(), TAM_BULLET_X, TAM_BULLET_Y };
			b = { (int)shi.pos.getX(), (int)shi.pos.getY(), TAM_SHIELD_X, TAM_SHIELD_Y };

			if (SDL_HasIntersection(&a, &b))
			{
				//Borrado de escudo y bala
				shieldsDelete.push_back((*it2).second);
				bulletsDelete.push_back((*it).second);
			}
		}

		for (auto it3 = players.begin(); it3 != players.end(); ++it3)
		{
			SDL_Rect a, b;
			GOInfo* bul = (*it).second;
			GOInfo pla = (*it3);
			a = { (int)bul->pos.getX(), (int)bul->pos.getY(), TAM_BULLET_X, TAM_BULLET_Y };
			b = { (int)pla.pos.getX(), (int)pla.pos.getY(), TAM_JUG_X, TAM_JUG_Y };

			if (SDL_HasIntersection(&a, &b))
			{
				//Borrado de jugador y bala
				bulletsDelete.push_back((*it).second);
				Message msg = Message(MessageType::PLAYERDEAD, &(*it3));

				for (auto it = clients.begin(); it != clients.end(); ++it)
					if (socket.send(msg, *(*it)) == -1)  std::cout << "ERROR: 9\n";


			}
		}
	}

	for (auto o = bulletsDelete.begin(); o != bulletsDelete.end(); ++o) {
		Message msg = Message(MessageType::BORRABALA, *o);

		for (auto it = clients.begin(); it != clients.end(); ++it)
			if (socket.send(msg, *(*it)) == -1)std::cout << "ERROR: 7\n";

		bullets.erase((*o)->id);
	}

	for (auto o = shieldsDelete.begin(); o != shieldsDelete.end(); ++o) {
		Message msg = Message(MessageType::BORRAESCUDO, &(*o));

		for (auto it = clients.begin(); it != clients.end(); ++it)
			if (socket.send(msg, *(*it)) == -1)  std::cout << "ERROR: 8 \n";;

		shields.erase((*o).id);
	}
}