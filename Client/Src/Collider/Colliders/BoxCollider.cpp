#include "BoxCollider.h"

BoxCollider::BoxCollider():
	BoxCollider(glm::vec3(0.0f), glm::vec3(1.0f))
{
}

BoxCollider::BoxCollider(const glm::vec3& pos):
	BoxCollider(pos, glm::vec3(1.0f))
{
}

BoxCollider::BoxCollider(const glm::vec3& scale):
	BoxCollider(glm::vec3(0.0f), scale)
{
}

BoxCollider::BoxCollider(const glm::vec3& pos, const glm::vec3& scale):
	Collider(ColliderType::Box),
	pos(pos),
	scale(scale)
{
}