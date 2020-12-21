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

void SphereCollider::Reset(){
	type = ColliderType::Sphere;
	radius = 0.0f;
	pos = glm::vec3();
}

float SphereCollider::GetRadius() const{
	return radius;
}

const glm::vec3& SphereCollider::GetPos() const{
	return pos;
}

void SphereCollider::SetRadius(const float radius){
	this->radius = radius;
}

void SphereCollider::SetPos(const glm::vec3& pos){
	this->pos = pos;
}