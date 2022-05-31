#include "Socket.h"
#include <map>
#include "GOInfo.h"
#include <vector>
#include <memory>
#include <mutex>
#include <list>

class GameServer
{
public:
    GameServer(const char * s, const char * p);
    
    void do_messages();   
    void move_bullets();   
    void collisions();
    bool fin = false;


private:
    std::vector<std::unique_ptr<Socket>> clients;
    std::vector<GOInfo> players;
    std::map<int16_t,GOInfo> shields;
    std::map<int16_t,GOInfo*> bullets;

    const float timeUpdate = 10.0f;
    float initTime = 0.0f;
    
    int nPlayers = 0;
    int nShields = 0;
    int nBullets = 0;


    int nPlayerDelete = -1;
    Socket socket;
};