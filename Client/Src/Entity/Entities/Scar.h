#pragma once

#include "Gun.h"

class Scar final: public Gun{
public:
	Scar();
	void Shoot(const float& elapsedTime, const glm::vec3& camPos, const glm::vec3& camFront, ISoundEngine* const& soundEngine) override;
};