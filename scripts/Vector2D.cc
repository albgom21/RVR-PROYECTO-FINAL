#include "Vector2D.h"

#pragma region Operators
void Vector2D::normalize() {
	double mag = magnitude();
	if (mag > 0.0) {
		x_ = x_ / mag;
		y_ = y_ / mag;
	}
}
#pragma endregion