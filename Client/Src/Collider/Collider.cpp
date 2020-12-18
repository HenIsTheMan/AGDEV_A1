#include "Collider.h"

bool Collider::GetActive() const{
	return active;
}

ColliderType Collider::GetType() const{
	return type;
}

void Collider::SetActive(const bool active){
	this->active = active;
}

void Collider::SetType(const ColliderType type){
	this->type = type;
}

Collider::Collider():
	Collider(ColliderType::Amt)
{
}

Collider::Collider(const ColliderType type):
	active(false),
	type(type)
{
}