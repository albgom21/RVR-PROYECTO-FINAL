#pragma once
#include "Vector2D.h"

class Texture;
class Player {
private:
    Texture* _txt, *_txtShield;
    Vector2D _pos;
    int16_t _num, _numShield;

public:
    Player(Texture* txt, Texture* txtShield, Vector2D pos, int16_t num);
    ~Player(){}

    bool canShield() { return (_numShield < 3); }

    void setPos(Vector2D pos) { _pos = pos; }
    void setNum(int16_t num) { _num = num; }
    void addShield() { _numShield++; }

    Vector2D getPos() { return _pos; }
    int16_t getNum() { return _num; }
    Texture* getTexture() { return _txt; }

};