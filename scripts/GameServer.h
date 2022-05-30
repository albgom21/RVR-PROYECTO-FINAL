#include "Socket.h"
#include <map>
#include "GOInfo.h"
#include <vector>
#include <memory>
#include <mutex>

class GameServer
{
public:
    GameServer(const char * s, const char * p);
    
    void do_messages();   
    void move_bullets();

private:
    std::vector<std::unique_ptr<Socket>> clients;
    std::vector<GOInfo> players;
    std::vector<GOInfo> shields;
    std::map<std::string, GOInfo*> bullets;
    
    int nPlayers = 0;
    int nShields = 0;
    int nBullets = 0;
    Socket socket;
};