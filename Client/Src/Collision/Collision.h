#pragma once

#include "../Entity/Entity.h"

#include "../Shared/Axis.hpp"

class Collision final{ //Static class
public:
	static bool DetectCollision(const Entity* const actor, const Entity* const actee);
private:
	static bool DetectRayBoxIntersection(const glm::vec3& rayStart, const glm::vec3& rayEnd, const glm::vec3& minAABB, const glm::vec3& maxAABB, glm::vec3& collisionPt);
	static bool CalcIntersection(const float dist0, const float dist1, const glm::vec3& rayStart, const glm::vec3& rayEnd, glm::vec3& collisionPt);
	static bool WithinBox(const Axis axis, const glm::vec3& minAABB, const glm::vec3& maxAABB, glm::vec3& collisionPt);

	Collision() = delete;
	~Collision() = delete;
};