#include <string>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <memory>

class Game;
class Player;

class GameClient {
private:
    Player* _myPlayer = nullptr;
    Game*   _app = nullptr;
    float _y;

public: 
    GameClient();
    void initClient();
    void render();
    void input();
    void run();
};