#pragma once

#include <Engine.h>

#include "Colliders/BoxCollider.h"
#include "Colliders/SphereCollider.h"

#include "../../ObjPool/ObjPool.h"

class ColliderManager final: public Singleton<ColliderManager>{
	friend Singleton<ColliderManager>;
public:
	~ColliderManager();

	void InitBoxColliderPool(const size_t& inactiveSize, const size_t& activeSize);
	void InitSphereColliderPool(const size_t& inactiveSize, const size_t& activeSize);

	Collider* ActivateCollider(const ColliderType type);
	void DeactivateCollider(Collider* const collider);
private:
	ColliderManager();

	ObjPool<BoxCollider>* boxColliderPool;
	ObjPool<SphereCollider>* sphereColliderPool;
};