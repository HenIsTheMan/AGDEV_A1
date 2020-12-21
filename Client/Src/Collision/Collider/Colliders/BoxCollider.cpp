#include "BoxCollider.h"

BoxCollider::BoxCollider():
	BoxCollider(glm::vec3(0.0f), glm::vec3(1.0f))
{
}

BoxCollider::BoxCollider(const glm::vec3& pos, const glm::vec3& scale):
	Collider(ColliderType::Box),
	pos(pos),
	scale(scale)
{
}

float BoxCollider::CalcDiagLen(){
	return sqrt(scale.x * scale.x + scale.y * scale.y + scale.z * scale.z);
}

void BoxCollider::Reset(){
	type = ColliderType::Box;
	pos = glm::vec3();
	scale = glm::vec3(1.0f);
}

const glm::vec3& BoxCollider::GetPos() const{
	return pos;
}

const glm::vec3& BoxCollider::GetScale() const{
	return scale;
}

void BoxCollider::SetPos(const glm::vec3& pos){
	this->pos = pos;
}

void BoxCollider::SetScale(const glm::vec3& scale){
	this->scale = scale;
}