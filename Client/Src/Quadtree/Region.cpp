#include "Region.h"

Region::Region():
	parent(nullptr),
	origin(glm::vec3(0.f)),
	size(glm::vec3(0.f)),
	UL(nullptr),
	UR(nullptr),
	DL(nullptr),
	DR(nullptr),
	entities(new std::vector<Entity*>()),
	active(false)
{
}

Region::~Region(){
	if(entities){
		for(Entity*& entity: *entities){
			if(entity){ //Deleted in EntityManager
				entity = nullptr;
			}
		}
		delete entities;
		entities = nullptr;
	}
}

void Region::AddEntity(Entity* const entity){
	entities->emplace_back(entity);
}

void Region::DeactivateAndClear(){
	if(UL){
		UL->active = false;
		UL->entities->clear();
		UL->DeactivateAndClear();
	}
	if(UR){
		UR->active = false;
		UR->entities->clear();
		UR->DeactivateAndClear();
	}
	if(DL){
		DL->active = false;
		DL->entities->clear();
		DL->DeactivateAndClear();
	}
	if(DR){
		DR->active = false;
		DR->entities->clear();
		DR->DeactivateAndClear();
	}
}

void Region::Partition(){
	if(entities->size() > 7){
		///Set up UL
		UL = FetchRegion();
		UL->active = true;
		UL->SetParent(this);
		UL->SetOrigin(glm::vec3(origin.x - size.x / 4.f, origin.y, origin.z - size.z / 4.f));
		UL->SetSize(glm::vec3(size.x / 2.f, size.y, size.z / 2.f));

		///Set up UR
		UR = FetchRegion();
		UR->active = true;
		UR->SetParent(this);
		UR->SetOrigin(glm::vec3(origin.x + size.x / 4.f, origin.y, origin.z - size.z / 4.f));
		UR->SetSize(glm::vec3(size.x / 2.f, size.y, size.z / 2.f));

		///Set up DL
		DL = FetchRegion();
		DL->active = true;
		DL->SetParent(this);
		DL->SetOrigin(glm::vec3(origin.x - size.x / 4.f, origin.y, origin.z + size.z / 4.f));
		DL->SetSize(glm::vec3(size.x / 2.f, size.y, size.z / 2.f));

		///Set up DR
		DR = FetchRegion();
		DR->active = true;
		DR->SetParent(this);
		DR->SetOrigin(glm::vec3(origin.x + size.x / 4.f, origin.y, origin.z + size.z / 4.f));
		DR->SetSize(glm::vec3(size.x / 2.f, size.y, size.z / 2.f));

		for(Entity*& entity: *entities){
			if(entity && entity->active
				&& entity->pos.x + entity->scale.x <= origin.x + size.x / 2.f && entity->pos.x - entity->scale.x >= origin.x - size.x / 2.f
				&& entity->pos.y + entity->scale.y <= origin.y + size.y / 2.f && entity->pos.y - entity->scale.y >= origin.y - size.y / 2.f
				&& entity->pos.z + entity->scale.z <= origin.z + size.z / 2.f && entity->pos.z - entity->scale.z >= origin.z - size.z / 2.f
				){
				if(entity->pos.z - entity->scale.z <= origin.z){
					if(entity->pos.x - entity->scale.x <= origin.x){
						UR->AddEntity(entity);
					}
					if(entity->pos.x + entity->scale.x >= origin.x){
						UL->AddEntity(entity);
					}
				}
				if(entity->pos.z + entity->scale.z >= origin.z){
					if(entity->pos.x - entity->scale.x <= origin.x){
						DR->AddEntity(entity);
					}
					if(entity->pos.x + entity->scale.x >= origin.x){
						DL->AddEntity(entity);
					}
				}
			}
		}

		///Use recursion to continue forming the Quadtree
		UL->Partition();
		UR->Partition();
		DL->Partition();
		DR->Partition();
	}
}

const Region* Region::FindEntity(Entity* const entity) const{ //Search children 1st
	if(UL){ //If UL exists...
		const Region* const& Region = UL->FindEntity(entity);
		if(Region){ //If entity was found in a Region...
			return Region;
		}
	}
	if(UR){ //If UR exists...
		const Region* const& Region = UR->FindEntity(entity);
		if(Region){ //If entity was found in a Region...
			return Region;
		}
	}
	if(DL){ //If DL exists...
		const Region* const& Region = DL->FindEntity(entity);
		if(Region){ //If entity was found in a Region...
			return Region;
		}
	}
	if(DR){ //If DR exists...
		const Region* const& Region = DR->FindEntity(entity);
		if(Region){ //If entity was found in a Region...
			return Region;
		}
	}
	if(entities){
		for(Entity*& myEntity: *entities){
			if(myEntity == entity){
				return this;
			}
		}
	}
	return nullptr;
}

const glm::vec3& Region::GetOrigin() const{
	return origin;
}

const glm::vec3& Region::GetSize() const{
	return size;
}

void Region::SetParent(Region* const parent){
	this->parent = parent;
}

void Region::SetOrigin(const glm::vec3& origin){
	this->origin = origin;
}

void Region::SetSize(const glm::vec3& size){
	this->size = size;
}

void Region::SetActive(const bool active){
	this->active = active;
}

void Region::InitRegionPool(const int& amt){
	RegionPool = std::vector<Region*>(amt);
	for(int i = 0; i < amt; ++i){
		RegionPool[i] = new Region();
	}
}

void Region::DestroyRegionPool(){
	const size_t& size = RegionPool.size();
	for(size_t i = 0; i < size; ++i){
		if(RegionPool[i]){
			delete RegionPool[i];
			RegionPool[i] = nullptr;
		}
	}
}

Region* const& Region::FetchRegion(){
	for(Region* const& Region: RegionPool){
		if(!Region->active){
			return Region;
		}
	}
	RegionPool.emplace_back(new Region());
	(void)puts("1 cube section was added to RegionPool!");
	return RegionPool.back();
}