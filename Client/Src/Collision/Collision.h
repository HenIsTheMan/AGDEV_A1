#pragma once

#include "../Entity/Entity.h"

class Collision final{ //Static class
public:
	static bool DetectCollision(const Entity* const actor, const Entity* const actee);
private:
	Collision() = delete;
};