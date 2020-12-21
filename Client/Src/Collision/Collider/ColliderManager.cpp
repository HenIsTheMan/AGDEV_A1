#include "ColliderManager.h"

ColliderManager::~ColliderManager(){
	if(boxColliderPool != nullptr){
		boxColliderPool->Destroy();
		boxColliderPool = nullptr;
	}

	if(sphereColliderPool != nullptr){
		sphereColliderPool->Destroy();
		sphereColliderPool = nullptr;
	}
}

void ColliderManager::InitBoxColliderPool(const size_t& inactiveSize, const size_t& activeSize){
	boxColliderPool->Init(inactiveSize, activeSize);
}

void ColliderManager::InitSphereColliderPool(const size_t& inactiveSize, const size_t& activeSize){
	sphereColliderPool->Init(inactiveSize, activeSize);
}

Collider* ColliderManager::ActivateCollider(const ColliderType type){
	if(type == ColliderType::Box){
		BoxCollider* const boxCollider = boxColliderPool->ActivateObj();
		boxCollider->Reset();
		return boxCollider;
	} else{
		SphereCollider* const sphereCollider = sphereColliderPool->ActivateObj();
		sphereCollider->Reset();
		return sphereCollider;
	}
}

void ColliderManager::DeactivateCollider(Collider* const collider){
	if(collider->GetType() == ColliderType::Box){
		boxColliderPool->DeactivateObj((BoxCollider*)collider);
	} else{
		sphereColliderPool->DeactivateObj((SphereCollider*)collider);
	}
}

ColliderManager::ColliderManager():
	boxColliderPool(ObjPool<BoxCollider>::GetObjPtr()),
	sphereColliderPool(ObjPool<SphereCollider>::GetObjPtr())
{
}