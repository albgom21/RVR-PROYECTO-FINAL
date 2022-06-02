#pragma once
#include "Vector2D.h"
#include "Constants.h"

class Texture;
class Player {
private:
	Texture* _txt; 				// Textura del jugador.
	Vector2D _pos;				// Posición del jugador.
	int16_t _num, _numShield;   // N del jugador y cantidad de escudos activos.

public:
	// Constructora.
	Player(Texture* txt, int16_t num) : _txt(txt), _num(num), _numShield(0) {};
	// Destructora.
	~Player() {}

	// Comprobar si se puede añadir un nuevo escudo.
	bool canShield() { return (_numShield < N_ESCUDOS); }

	// Añadir y quitar escudos del contador.
	void addShield() { _numShield++; }
	void subShield() { _numShield--; }

	// Setters y Getters.
	void setPos(Vector2D pos) { _pos = pos; }
	void setNum(int16_t num) { _num = num; }

	int16_t getNumShield() { return _numShield; }
	Vector2D getPos() { return _pos; }
	int16_t getNum() { return _num; }
	Texture* getTexture() { return _txt; }
};