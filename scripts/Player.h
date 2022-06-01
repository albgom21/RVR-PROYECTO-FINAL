#pragma once
#include "Vector2D.h"
#include "Constants.h"

class Texture;
class Player {
private:
    Texture* _txt, *_txtShield;
    Vector2D _pos;
    int16_t _num, _numShield;

public:
    Player(Texture* txt,  int16_t num): _txt(txt),_num(num), _numShield(0) {};
    ~Player(){}

    bool canShield() { return (_numShield < N_ESCUDOS); }

    void setPos(Vector2D pos) { _pos = pos; }
    void setNum(int16_t num) { _num = num; }
    void addShield() { _numShield++; }
    void subShield() { _numShield--; }

    int16_t getNumShield() {return _numShield;}
    Vector2D getPos() { return _pos; }
    int16_t getNum() { return _num; }
    Texture* getTexture() { return _txt; }

};