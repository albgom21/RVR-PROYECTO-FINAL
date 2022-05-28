#include <string>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <memory>
#include "Socket.h"
#include "GOInfo.h"

class Game;
class Player;

class GameClient {
private:
    Player* _myPlayer = nullptr;
    Game*   _app = nullptr;
    Socket socket;
    bool playing = true;
    bool other = false;
    GOInfo _otherPlayer;


public: 
    GameClient(const char *s, const char *p, const char *n);
    ~GameClient();
    void initClient();
    void render();    
    void net_thread();
    void input();
    void run();
};