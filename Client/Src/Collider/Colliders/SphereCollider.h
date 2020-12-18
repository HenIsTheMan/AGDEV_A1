#pragma once

#include <Core.h>

#include "../Collider.h"

class SphereCollider final: public Collider{
public:
	SphereCollider();
	SphereCollider(const float radius);
	SphereCollider(const glm::vec3& pos);
	SphereCollider(const float radius, const glm::vec3& pos);
private:
	float radius;
	glm::vec3 pos;
};