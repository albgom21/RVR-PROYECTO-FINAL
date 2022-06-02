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

GameServer::~GameServer()
{	// Borrado de balas.
	for(auto b: bullets)
		delete b.second;
}

void GameServer::do_messages()
{
	if (socket.bind() == -1)
		std::cout << "ERROR: en Bind\n";

	while (true)
	{
		Message cm;
		Socket* s = nullptr;

		// RECEPCIÓN DE MENSAJES -----------------------------------------------------
		if (socket.recv(cm, s) == -1)
			std::cout << "ERROR: no se pudo recibir el mensaje\n";
	
		switch (cm.getMessageType()) {
		// Nueva conexión de cliente.
		case MessageType::LOGIN:
		{
			// Almacenar el nuevo cliente.
			clients.push_back(std::move(std::make_unique<Socket>(*s)));
			GOInfo n;
			n.nJug = nPlayers;
			std::cout << "NPLAYERS: " << n.nJug << "\n";

			// Posicionar el jugador según el orden de conexión.
			if (nPlayers == 0)
				n.pos = Vector2D(X_INI, Y_INI);
			else
				n.pos = Vector2D(X_INI_2, Y_INI);

			// Almacenar el nuevo jugador.
			players.push_back(n);
			
			Message msg = Message(MessageType::NEWPLAYER, &players[nPlayers]);
			// Enviar el mensaje al resto de jugadores.
			for (auto it = clients.begin(); it != clients.end(); it++) {
				if (*((*it).get()) != *s)
					if (socket.send(msg, *(*it)) == -1)  std::cout << "ERROR: 0\n";;
			}

			// Enviar la información del resto de jugadores al nuevo jugador.
			for (auto it = players.begin(); it != players.end(); ++it)
			{
				if ((*it).nJug != cm.getGOInfo().nJug)
				{
					msg.setGOInfo(*it);
					if (socket.send(msg, *s) == -1)  std::cout << "ERROR: 1\n";;
				}
			}

			nPlayers++;
			break;
		}
		// Cliente desconectado.
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
		// Actualizar la posición de un jugador.
		case MessageType::PLAYERPOS:
		{
			players[cm.getGOInfo().nJug] = cm.getGOInfo();

			// Enviar el mensaje al resto de jugadores.
			for (auto it = clients.begin(); it != clients.end(); it++)			
				if (*((*it).get()) != *s)
					if (socket.send(cm, (*((*it).get()))) == -1)  std::cout << "ERROR: 2\n";;
			

			break;
		}
		// Nuevo escudo.
		case MessageType::ESCUDO:
		{
			GOInfo obj;
			// Offset dependiendo de que jugador sea.
			int offset = 110;
			if (cm.getGOInfo().nJug == 1) offset = -(-TAM_JUG_X + 110 + TAM_SHIELD_X);

			// Crear escudo.
			obj.pos = Vector2D(cm.getGOInfo().pos.getX() + offset, cm.getGOInfo().pos.getY() + (TAM_JUG_Y / 2) - (TAM_SHIELD_Y / 2));
			obj.nJug = cm.getGOInfo().nJug;
			obj.id = nShields;

			bool correctPos = true;
			// Ver que al crear un escudo no colisione con otro.
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
				// Enviar el mensaje a todos los jugadores.
				for (auto i = clients.begin(); i != clients.end(); ++i)
					if (socket.send(cm, (*(*i))) == -1) std::cout << "ERROR: 3\n";
			}

			break;
		}
		// Nueva bala.
		case MessageType::DISPARO:
		{
			GOInfo* obj = new GOInfo();
			// Offset dependiendo de que jugador sea.
			int offset = 50;
			if (cm.getGOInfo().nJug == 1)  offset = -(-TAM_JUG_X + 50 + TAM_BULLET_X);

			// Crear y almacenar bala.
			obj->pos = Vector2D(cm.getGOInfo().pos.getX() + offset, cm.getGOInfo().pos.getY() + (TAM_JUG_Y / 2) - (TAM_BULLET_Y / 2));
			obj->nJug = cm.getGOInfo().nJug;
			obj->id = nBullets;
			bullets[nBullets] = obj;
			nBullets++;
			// Enviar el mensaje a todos los jugadores.
			Message cm = Message(MessageType::NEWBALA, obj);
			for (auto i = clients.begin(); i != clients.end(); ++i)
				if (socket.send(cm, (*(*i))) == -1)  std::cout << "ERROR: 4\n";;

			break;
		}
		default:
			break;
		}
		// Liberar socket si no se usa.
		if (cm.getMessageType() != MessageType::LOGIN) delete s;
	}
}

void GameServer::move_bullets() {
	// Mover las balas cada X tiempo.
	if (SDL_GetTicks() - initTime > timeUpdate)
	{
		std::list<GOInfo*> bulletsDelete;
		double_t x = 0;
		for (auto b : bullets) {
			b.second->nJug == 1 ? x = -VELOCITY : x = VELOCITY;
			// Comprobar si se ha salido la bala por la izquierda.
			if (x < 0 && b.second->pos.getX() < -TAM_BULLET_X)
				bulletsDelete.push_back(b.second);
			// Comprobar si se ha salido la bala por la derecha.
			else if (x > 0 && b.second->pos.getX() > W_WIDTH)
				bulletsDelete.push_back(b.second);
			// Si no se ha salido la bala, actualizar su posición.
			else {
				b.second->pos.setX(b.second->pos.getX() + x);
				// Enviar el mensaje a todos los jugadores.
				Message msg = Message(MessageType::BALAPOS, b.second);
				for (auto it = clients.begin(); it != clients.end(); ++it)
					if (socket.send(msg, *(*it)) == -1)  std::cout << "ERROR: 5\n";;
			}
		}
		// Borrar las balas que se salieron de los límites.
		for (auto o = bulletsDelete.begin(); o != bulletsDelete.end(); ++o) {
			// Enviar el mensaje a todos los jugadores.
			Message msg = Message(MessageType::BORRABALA, *o);
			for (auto it = clients.begin(); it != clients.end(); ++it)
				if (socket.send(msg, *(*it)) == -1)  std::cout << "ERROR: 6\n";;

			bullets.erase((*o)->id);
		}
		// Reiniciar el tiempo.
		initTime = SDL_GetTicks();
	}
}

void GameServer::collisions()
{
	std::list<GOInfo*> bulletsDelete;
	std::list<GOInfo> shieldsDelete;
	
	// Recorrido de las balas.
	for (auto it = bullets.begin(); it != bullets.end(); ++it)
	{
		// Recorrido de los escudos.
		for (auto it2 = shields.begin(); it2 != shields.end(); ++it2)
		{
			SDL_Rect a, b;
			GOInfo* bul = (*it).second;
			GOInfo shi = (*it2).second;
			a = { (int)bul->pos.getX(), (int)bul->pos.getY(), TAM_BULLET_X, TAM_BULLET_Y };
			b = { (int)shi.pos.getX(), (int)shi.pos.getY(), TAM_SHIELD_X, TAM_SHIELD_Y };

			//Comprobar la colisión entre balas y escudos.
			if (SDL_HasIntersection(&a, &b))
			{
				//Borrado de escudo y bala.
				shieldsDelete.push_back((*it2).second);
				bulletsDelete.push_back((*it).second);
			}
		}
		// Recorrido de jugadores.
		for (auto it3 = players.begin(); it3 != players.end(); ++it3)
		{
			SDL_Rect a, b;
			GOInfo* bul = (*it).second;
			GOInfo pla = (*it3);
			a = { (int)bul->pos.getX(), (int)bul->pos.getY(), TAM_BULLET_X, TAM_BULLET_Y };
			b = { (int)pla.pos.getX(), (int)pla.pos.getY(), TAM_JUG_X, TAM_JUG_Y };

			//Comprobar la colisión entre balas y jugadores.
			if (SDL_HasIntersection(&a, &b))
			{
				//Borrado bala
				bulletsDelete.push_back((*it).second);
				// Enviar a todos el mensaje que termina el juego.
				Message msg = Message(MessageType::PLAYERDEAD, &(*it3));
				for (auto it = clients.begin(); it != clients.end(); ++it)
					if (socket.send(msg, *(*it)) == -1)  std::cout << "ERROR: 9\n";
			}
		}
	}
	// Borrado de balas.
	for (auto o = bulletsDelete.begin(); o != bulletsDelete.end(); ++o) {
		// Enviar el mensaje a todos los jugadores.
		Message msg = Message(MessageType::BORRABALA, *o);
		for (auto it = clients.begin(); it != clients.end(); ++it)
			if (socket.send(msg, *(*it)) == -1)std::cout << "ERROR: 7\n";

		bullets.erase((*o)->id);
	}
	// Borrado de escudos.
	for (auto o = shieldsDelete.begin(); o != shieldsDelete.end(); ++o) {
		// Enviar el mensaje a todos los jugadores.
		Message msg = Message(MessageType::BORRAESCUDO, &(*o));
		for (auto it = clients.begin(); it != clients.end(); ++it)
			if (socket.send(msg, *(*it)) == -1)  std::cout << "ERROR: 8 \n";;

		shields.erase((*o).id);
	}
}