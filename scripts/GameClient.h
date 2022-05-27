#include <string>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <memory>
#include "Socket.h"

class Game;
class Player;
class GOInfo;

class GameClient {
private:
    Player* _myPlayer = nullptr;
    Game*   _app = nullptr;
    float _y;
    Socket socket;
    bool playing = true;
    std::vector<GOInfo> players;


public: 
    GameClient(const char *s, const char *p);
    ~GameClient();
    void initClient();
    void render();    
    void net_thread();
    void input();
    void run();
};