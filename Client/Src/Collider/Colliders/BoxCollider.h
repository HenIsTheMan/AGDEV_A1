#pragma once

#include <Core.h>

#include "../Collider.h"

class BoxCollider final: public Collider{
public:
	BoxCollider();
	BoxCollider(const glm::vec3& pos);
	BoxCollider(const glm::vec3& scale);
	BoxCollider(const glm::vec3& pos, const glm::vec3& scale);
private:
	glm::vec3 pos;
	glm::vec3 scale;
};