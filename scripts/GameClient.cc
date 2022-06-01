#include "GameClient.h"
#include "Game.h"
#include "Player.h"
#include "InputHandler.h"
#include "Resources.h" 
#include "Constants.h"
#include "Message.h"
#include "GOInfo.h"

GameClient::GameClient(const char* s, const char* p, const char* n) : socket(s, p) {
	_app = Game::GetInstance();
	lastShoot = SDL_GetTicks();

	// Cargamos la textura.
	for (auto& image : Resources::imageRoutes)
		_app->getTextureManager()->loadFromImg(image.id, _app->getRenderer(), image.route);

	// Instanciamos el jugador.
	_myPlayer = new Player(_app->getTextureManager()->getTexture(Resources::ID::P1), std::strtol(n, NULL, 10));
}

GameClient::~GameClient() {
	delete _myPlayer;
	delete _app;
}

void GameClient::initClient() {
	// Informamos al servidor de que se ha conectado un cliente.
	Message m = Message(MessageType::LOGIN, _myPlayer);
	if (socket.send(m, socket) == -1)
		std::cout << "ERROR: no se pudo enviar el mensaje de LOGIN\n";
}

void GameClient::render() {
	SDL_RenderClear(_app->getRenderer());

	// RENDER DE TEXTURAS -----------------------------------------------------
	// Fondo.
	Texture* t = _app->getTextureManager()->getTexture(Resources::ID::BACKGROUND);
	t->render({ 0, 0, W_WIDTH, W_HEIGHT });

	// Jugador.
	_myPlayer->getTexture()->render({ (int)_myPlayer->getPos().getX(), (int)_myPlayer->getPos().getY(),TAM_JUG_X, TAM_JUG_Y });

	// Conexión de un nuevo jugador.
	if (other) {
		t = _app->getTextureManager()->getTexture(Resources::ID::P2);
		t->render({ (int)_otherPlayer.pos.getX(), (int)_otherPlayer.pos.getY(), TAM_JUG_X, TAM_JUG_Y });
	}

	// Escudos.
	t = _app->getTextureManager()->getTexture(Resources::ID::SHIELD);
	for (auto it = shields.begin(); it != shields.end(); ++it)
	{
		GOInfo s = (*it).second;
		t->render({ (int)s.pos.getX(), (int)s.pos.getY(), TAM_SHIELD_X, TAM_SHIELD_Y });
	}

	// Balas.
	t = _app->getTextureManager()->getTexture(Resources::ID::BULLET);
	for (auto it = bullets.begin(); it != bullets.end(); ++it) {
		GOInfo b = (*it).second;
		t->render({ (int)b.pos.getX(), (int)b.pos.getY(), TAM_SHIELD_Y, TAM_SHIELD_X }); // PONER CONSTANTES
	}

	SDL_RenderPresent(_app->getRenderer());
}

void GameClient::net_thread()
{
	while (playing)
	{
		Message m;
		socket.recv(m);

		// RECEPCIÓN DE MENSAJES -----------------------------------------------------
		switch (m.getMessageType())
		{
		// Nuevo jugador conectado.
		case MessageType::NEWPLAYER:
		{
			GOInfo p = m.getGOInfo();
			if (p.nJug != _myPlayer->getNum()) {
				_otherPlayer = p;
				other = true;
			}
			else
			{
				_myPlayer->setPos(p.pos);
				_myPlayer->setNum(p.nJug);
			}

			break;
		}
		// Posición de jugador.
		case MessageType::PLAYERPOS:
		{
			GOInfo p = m.getGOInfo();
			_otherPlayer = p;

			break;
		}
		// Posición de bala.
		case MessageType::BALAPOS:
		{
			GOInfo b = m.getGOInfo();
			bullets[b.id] = b;

			break;
		}
		// Muerte de jugador.
		case MessageType::PLAYERDEAD:
		{
			if (m.getGOInfo().nJug == _myPlayer->getNum())
				std::cout << "GAME OVER\n";
			else
				std::cout << "YOU WIN!\n";

			m.setMsgType(MessageType::LOGOUT);
			socket.send(m, socket);


			playing = false;

			break;
		}
		// Nuevo escudo.
		case MessageType::NEWESCUDO:
		{
			GOInfo s = m.getGOInfo();
			if (s.nJug == _myPlayer->getNum())
				_myPlayer->addShield();
			shields[s.id] = s;
			break;
		}
		// Nueva bala.
		case MessageType::NEWBALA:
		{
			GOInfo b = m.getGOInfo();
			bullets[b.id] = b;
			break;
		}
		// Borrar una bala.
		case MessageType::BORRABALA:
		{
			bullets.erase(m.getGOInfo().id);
			break;
		}
		// Borrar un escudo.
		case MessageType::BORRAESCUDO:
		{
			GOInfo s = m.getGOInfo();
			if (s.nJug == _myPlayer->getNum())
				_myPlayer->subShield();
			shields.erase(m.getGOInfo().id);
			break;
		}
		}
	}
}


void GameClient::input() {
	bool posM = false;
	bool escudoM = false;
	bool balaM = false;
	HandleEvents::instance()->update();

	// LECTURA DE INPUT -----------------------------------------------------
	// Movimiento positivo en el eje vertical.
	if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_W) && (_myPlayer->getPos().getY() - VELOCITY) >= 0) {
		_myPlayer->setPos({ _myPlayer->getPos().getX(), _myPlayer->getPos().getY() - VELOCITY });
		posM = true;
	}
	// Movimiento negativo en el eje vertical.
	else if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_S) && (_myPlayer->getPos().getY() + VELOCITY + TAM_JUG_Y) <= W_HEIGHT) {
		_myPlayer->setPos({ _myPlayer->getPos().getX(), _myPlayer->getPos().getY() + VELOCITY });
		posM = true;
	}
	// Creación de escudo.
	else if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_X))
	{
		if (_myPlayer->canShield())
			escudoM = true;
	}
	// Creación de bala.
	else if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_SPACE) && SDL_GetTicks() - lastShoot > delay)
	{
		balaM = true;
		lastShoot = SDL_GetTicks();
	}

	// ENVÍO DE MENSAJES -----------------------------------------------------
	// Posición del jugador.
	if (posM && playing)
	{
		Message m(MessageType::PLAYERPOS, _myPlayer);
		socket.send(m, socket);
	}
	// Nuevo escudo.
	else if (escudoM && playing)
	{
		Message m(MessageType::ESCUDO, _myPlayer);
		socket.send(m, socket);
	}
	// Nueva bala.
	else if (balaM && playing)
	{
		Message m(MessageType::DISPARO, _myPlayer);
		socket.send(m, socket);
	}

}

void GameClient::run() {
	while (playing) {
		// Hasta que no se conecte el otro jugador, no empieza a leer el input.
		if (other) input();
		render();
	}
}