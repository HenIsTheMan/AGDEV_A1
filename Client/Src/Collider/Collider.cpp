#include "Collider.h"

bool Collider::GetActive() const{
	return active;
}

ColliderType Collider::GetType() const{
	return type;
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