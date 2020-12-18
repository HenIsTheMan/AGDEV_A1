#include "SphereCollider.h"

SphereCollider::SphereCollider():
	SphereCollider(0.0f, glm::vec3())
{
}

SphereCollider::SphereCollider(const float radius):
	SphereCollider(radius, glm::vec3())
{
}

SphereCollider::SphereCollider(const glm::vec3& pos):
	SphereCollider(0.0f, pos)
{
}

SphereCollider::SphereCollider(const float radius, const glm::vec3& pos):
	Collider(ColliderType::Sphere),
	radius(radius),
	pos(pos)
{
}