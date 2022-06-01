#include <string>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <memory>
#include <map>
#include "Socket.h"
#include "GOInfo.h"

class Game;
class Player;

// Cliente del juego.
class GameClient {
private:
    // Puntero al jugador del propio cliente.
    Player* _myPlayer = nullptr;
    // Puntero para iniciar la ventana y la carga de texturas.
    Game* _app = nullptr;
    // Vector para las balas.
    std::map<int16_t, GOInfo> bullets;
    // Vector para los escudos.
    std::map<int16_t, GOInfo> shields;

    // Socket del cliente.
    Socket socket;
    // Bool del bucle de juego.
    bool playing = true;
    // Bool para saber si hay otro jugador.
    bool other = false;
    // GOInfo del otro jugador.
    GOInfo _otherPlayer;

    // Cooldown para las balas
    const float delay = 750.0f;
    float lastShoot = 0.0f;

public:
    // Constructora del cliente.
    GameClient(const char* s, const char* p, const char* n);
    // Destructora del cliente.
    ~GameClient();
    // Inicialización del cliente.
    void initClient();
    // Render del cliente.
    void render();
    // Receptor de mensajes.
    void net_thread();
    // Input del cliente.
    void input();
    // Bucle principal del cliente.
    void run();
};