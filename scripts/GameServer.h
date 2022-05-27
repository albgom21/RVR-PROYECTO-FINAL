#include "Socket.h"
#include <map>
#include "GOInfo.h"
#include <vector>
#include <memory>

class GameServer
{
public:
    GameServer(const char * s, const char * p);
    
    void do_messages();   

private:
    std::vector<std::unique_ptr<Socket>> clients;
    std::vector<GOInfo> players;
    std::map<std::string,GOInfo> objects;
    
    int nPlayers = 0;
    Socket socket;
};