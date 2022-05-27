#include "GameServer.h"
#include <thread>

using namespace std;

int main(int argc, char **argv) {

    GameServer gs(argv[1], argv[2]);

    std::thread net_thread([&gs]() { gs.do_messages(); });
        
    return 0;
}