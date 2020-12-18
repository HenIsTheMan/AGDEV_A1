#include "Collider.h"

ColliderType Collider::GetType() const{
	return type;
}

void Collider::SetType(const ColliderType type){
	this->type = type;
}

Collider::Collider():
	Collider(ColliderType::Amt)
{
}

Collider::Collider(const ColliderType type):
	type(type)
{
}