#pragma once

#include <Engine.h>

#include "../Collider/Colliders/BoxCollider.h"
#include "../Collider/Colliders/SphereCollider.h"

class ColliderManager final: public Singleton<ColliderManager>{
	friend Singleton<ColliderManager>;
public:
	~ColliderManager();

	void Init(const size_t boxCollidersSize, const size_t sphereCollidersSize);

	Collider* ActivateCollider(const ColliderType type);
	void DeactivateCollider(Collider* const collider);
private:
	ColliderManager();

	std::vector<Collider*> boxColliders;
	std::vector<Collider*> sphereColliders;
};