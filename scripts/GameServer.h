#include "Socket.h"
#include <map>
#include "GOInfo.h"
#include <vector>
#include <memory>
#include <mutex>
#include <list>

// Servidor del juego.
class GameServer {
public:
	// Constructora del servidor.
	GameServer(const char* s, const char* p);
	// Destructora del servidor.
	~GameServer();
	// Creador de mensajes.
	void do_messages();
	// Movimiento de las balas.
	void move_bullets();
	// Comprobación de colisiones.
	void collisions();
	// Booleano para cerrar el servidor.
	bool fin = false;

private:
	// Vector para los clientes.
	std::vector<std::unique_ptr<Socket>> clients;
	// Vector para los jugadores.
	std::vector<GOInfo> players;
	// Mapa para los escudos.
	std::map<int16_t, GOInfo> shields;
	// Mapa para las balas.
	std::map<int16_t, GOInfo*> bullets;

	// Tiempo para actualizar las balas.
	const float timeUpdate = 16.66f;
	float initTime = 0.0f;

	// Contadores de jugadores, escudos y balas.
	int nPlayers = 0;
	int nShields = 0;
	int nBullets = 0;

	//Socket del servidor.
	Socket socket;
};