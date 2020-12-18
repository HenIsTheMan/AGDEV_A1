#include "ColliderManager.h"

void ColliderManager::Init(const size_t boxCollidersSize, const size_t sphereCollidersSize){
	boxColliders.reserve(boxCollidersSize);
	for(size_t i = 0; i < boxCollidersSize; ++i){
		boxColliders.emplace_back(new BoxCollider());
	}

	sphereColliders.reserve(sphereCollidersSize);
	for(size_t i = 0; i < sphereCollidersSize; ++i){
		boxColliders.emplace_back(new SphereCollider());
	}
}

Collider* ColliderManager::ActivateCollider(const ColliderType type){
	std::vector<Collider*>& colliders = type == ColliderType::Box ? boxColliders : sphereColliders;
	const size_t size = colliders.size();

	for(size_t i = 0; i < size; ++i){
		Collider* const collider = colliders[i];
		if(!collider->GetActive()){
			collider->SetActive(true);
			return collider;
		}
	}

	if(type == ColliderType::Box){
		colliders.emplace_back(new BoxCollider());
	} else{
		colliders.emplace_back(new SphereCollider());
	}
	(void)puts("A collider was added to the pool!\n");

	return colliders.back();
}

void ColliderManager::DeactivateCollider(Collider* const collider){
	collider->SetActive(false);
}

ColliderManager::ColliderManager():
	boxColliders(),
	sphereColliders()
{
}