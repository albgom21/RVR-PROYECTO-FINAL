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

class GameClient {
private:
    Player* _myPlayer = nullptr;
    Game*   _app = nullptr;
    std::map<int16_t,GOInfo> bullets;
    std::map<int16_t,GOInfo> shields;

    Socket socket;
    bool playing = true;
    bool other = false;
    GOInfo _otherPlayer;

    const float delay = 750.0f;
    float lastShoot = 0.0f;

public: 
    GameClient(const char *s, const char *p, const char *n);
    ~GameClient();
    void initClient();
    void render();    
    void net_thread();
    void input();
    void run();
};