#include "Collider.h"

ColliderType Collider::GetType() const{
	return type;
}

Collider::Collider():
	Collider(ColliderType::Amt)
{
}

Collider::Collider(const ColliderType type):
	type(type)
{
}