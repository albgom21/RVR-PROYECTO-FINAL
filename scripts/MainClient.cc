#include <thread>
#include "GameClient.h"


int main(int argc, char **argv)
{
    // 0.0.0.0 2222 0-1
    GameClient gc(argv[1],argv[2],argv[3]);
    //std::thread net_thread([&gc](){ gc.net_thread(); });
    std::thread([&gc]() { gc.net_thread(); }).detach();
    gc.initClient();
    gc.run();
    return 0;
}