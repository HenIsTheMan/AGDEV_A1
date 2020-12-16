#include "Region.h"

std::vector<Region*> Region::regionPool;

Region::Region():
	active(false),
	parent(nullptr),
	origin(glm::vec2()),
	size(glm::vec2()),
	topLeft(nullptr),
	topRight(nullptr),
	bottomLeft(nullptr),
	bottomRight(nullptr),
	stationaryEntities(),
	movableEntities()
{
}

Region::~Region(){
	for(Entity*& entity: stationaryEntities){
		if(entity){ //Deleted elsewhere
			entity = nullptr;
		}
	}

	for(Entity*& entity: movableEntities){
		if(entity){ //Deleted elsewhere
			entity = nullptr;
		}
	}
}

void Region::InitRegionPool(const size_t& size){
	if(regionPool.size()){
		assert(false && "regionPool is not empty!");
	}

	regionPool.resize(size);
	for(size_t i = 0; i < size; ++i){
		regionPool[i] = new Region();
	}
}

void Region::DestroyRegionPool(){
	for(Region*& region: regionPool){
		if(region){
			delete region;
			region = nullptr;
		}
	}
}

const Region* Region::FindEntity(Entity* const entity, const bool movable) const{
	if(topLeft){
		const Region* const& region = topLeft->FindEntity(entity, movable);
		if(region){
			return region;
		}
	}
	if(topRight){
		const Region* const& region = topRight->FindEntity(entity, movable);
		if(region){
			return region;
		}
	}
	if(bottomLeft){
		const Region* const& region = bottomLeft->FindEntity(entity, movable);
		if(region){
			return region;
		}
	}
	if(bottomRight){
		const Region* const& region = bottomRight->FindEntity(entity, movable);
		if(region){
			return region;
		}
	}

	const std::vector<Entity*>& entities = movable ? movableEntities : stationaryEntities;
	for(const Entity* const& myEntity: entities){
		if(myEntity == entity){
			return this;
		}
	}

	return nullptr;
}

Region* Region::FetchRegion(){
	for(Region* const region: regionPool){
		if(!region->active){
			return region;
		}
	}

	regionPool.emplace_back(new Region());
	(void)puts("A region was added to regionPool!");

	return regionPool.back();
}

void Region::AddEntity(Entity* const entity, const bool movable){
	(movable ? movableEntities : stationaryEntities).emplace_back(entity);
}

void Region::RemoveEntity(Entity* const entity, const bool movable){
	std::vector<Entity*>& entities = movable ? movableEntities : stationaryEntities;
	const std::vector<Entity*>::iterator iter = std::find(entities.begin(), entities.end(), entity);

	if(iter != entities.end()){
		return assert(false && "Entity could not be found!");
	}

	entities.erase(iter);
}

void Region::ClearMovableAndDeactivateChildren(){
	if(topLeft){
		topLeft->movableEntities.clear();
		topLeft->active = false;
		topLeft->ClearMovableAndDeactivateChildren();
	}
	if(topRight){
		topRight->movableEntities.clear();
		topRight->active = false;
		topRight->ClearMovableAndDeactivateChildren();
	}
	if(bottomLeft){
		bottomLeft->movableEntities.clear();
		bottomLeft->active = false;
		bottomLeft->ClearMovableAndDeactivateChildren();
	}
	if(bottomRight){
		bottomRight->movableEntities.clear();
		bottomRight->active = false;
		bottomRight->ClearMovableAndDeactivateChildren();
	}
}

void Region::Partition(const bool movable){
	const std::vector<Entity*>& entities = movable ? movableEntities : stationaryEntities;
	if(entities.size() <= (size_t)1){
		return;
	}

	topLeft = FetchRegion();
	topLeft->active = true;
	topLeft->parent = this;
	topLeft->origin = glm::vec2(origin[0] - size[0] * 0.25f, origin[1] - size[1] * 0.25f);
	topLeft->size = glm::vec2(size[0] * 0.5f, size[1] * 0.5f);

	topRight = FetchRegion();
	topRight->active = true;
	topRight->parent = this;
	topRight->origin = glm::vec2(origin[0] + size[0] * 0.25f, origin[1] - size[1] * 0.25f);
	topRight->size = topLeft->size;

	bottomLeft = FetchRegion();
	bottomLeft->active = true;
	bottomLeft->parent = this;
	bottomLeft->origin = glm::vec2(origin[0] - size[0] * 0.25f, origin[1] + size[1] * 0.25f);
	bottomLeft->size = topLeft->size;

	bottomRight = FetchRegion();
	bottomRight->active = true;
	bottomRight->parent = this;
	bottomRight->origin = glm::vec2(origin[0] + size[0] * 0.25f, origin[1] + size[1] * 0.25f);
	bottomRight->size = topLeft->size;

	//for(Entity*& entity: entities){
	//	if(entity
	//		&& entity->active
	//		&& entity->pos.x + entity->scale.x <= origin.x + size.x / 2.f && entity->pos.x - entity->scale.x >= origin.x - size.x / 2.f
	//		&& entity->pos.y + entity->scale.y <= origin.y + size.y / 2.f && entity->pos.y - entity->scale.y >= origin.y - size.y / 2.f
	//		&& entity->pos.z + entity->scale.z <= origin.z + size.z / 2.f && entity->pos.z - entity->scale.z >= origin.z - size.z / 2.f
	//	){
	//		if(entity->pos.z - entity->scale.z <= origin.z){
	//			if(entity->pos.x - entity->scale.x <= origin.x){
	//				topRight->AddEntity(entity);
	//			}
	//			if(entity->pos.x + entity->scale.x >= origin.x){
	//				topLeft->AddEntity(entity);
	//			}
	//		}
	//		if(entity->pos.z + entity->scale.z >= origin.z){
	//			if(entity->pos.x - entity->scale.x <= origin.x){
	//				bottomRight->AddEntity(entity);
	//			}
	//			if(entity->pos.x + entity->scale.x >= origin.x){
	//				bottomLeft->AddEntity(entity);
	//			}
	//		}
	//	}
	//}

	/////Use recursion to continue forming the Quadtree
	//topLeft->Partition();
	//topRight->Partition();
	//bottomLeft->Partition();
	//bottomRight->Partition();
}

void Region::ReserveStationaryEntities(const size_t& size){
	stationaryEntities.reserve(size);
}

void Region::ReserveMovableEntities(const size_t& size){
	movableEntities.reserve(size);
}