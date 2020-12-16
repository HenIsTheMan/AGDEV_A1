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
	stationaryNodes(),
	movableNodes()
{
}

Region::~Region(){
	for(Node*& node: stationaryNodes){
		if(node){
			delete node;
			node = nullptr;
		}
	}

	for(Node*& node: movableNodes){
		if(node){
			delete node;
			node = nullptr;
		}
	}
}

void Region::GetEntitiesToRender(std::map<int, Entity*>& entitiesOpaque, std::map<int, Entity*>& entitiesNotOpaque, const Cam& cam){
	if(topLeft){
		topLeft->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
	}
	if(topRight){
		topRight->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
	}
	if(bottomLeft){
		bottomLeft->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
	}
	if(bottomRight){
		bottomRight->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
	}

	const glm::vec3& camPos = cam.GetPos();
	const glm::vec3& camFront = cam.CalcFront();

	for(int i = 0; i < stationaryNodes.size(); ++i){
		Entity* const entity = stationaryNodes[i]->RetrieveEntity();

		if(entity && entity->active){
			glm::vec3 displacementVec = entity->pos - camPos;
			if(glm::dot(displacementVec, camFront) > 0.f){

				switch(entity->type){
					case Entity::EntityType::Bullet:
					case Entity::EntityType::Enemy:
					case Entity::EntityType::ShotgunAmmo:
					case Entity::EntityType::ScarAmmo:
					case Entity::EntityType::SniperAmmo:
						entitiesOpaque.insert(std::make_pair((int)glm::dot(displacementVec, displacementVec), entity));
						break;
					case Entity::EntityType::Coin:
					case Entity::EntityType::Fire:
						entitiesNotOpaque.insert(std::make_pair((int)glm::dot(displacementVec, displacementVec), entity));
						break;
				}
			}
		}
	}
}

const Region* Region::FindRegion(Node* const node, const bool movable) const{
	if(topLeft){
		const Region* const& region = topLeft->FindRegion(node, movable);
		if(region){
			return region;
		}
	}
	if(topRight){
		const Region* const& region = topRight->FindRegion(node, movable);
		if(region){
			return region;
		}
	}
	if(bottomLeft){
		const Region* const& region = bottomLeft->FindRegion(node, movable);
		if(region){
			return region;
		}
	}
	if(bottomRight){
		const Region* const& region = bottomRight->FindRegion(node, movable);
		if(region){
			return region;
		}
	}

	const std::vector<Node*>& nodes = movable ? movableNodes : stationaryNodes;
	for(const Node* const& myNode: nodes){
		if(myNode == node){
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

void Region::AddNode(Node* const node, const bool movable){
	(movable ? movableNodes : stationaryNodes).emplace_back(node);
}

void Region::RemoveNode(Node* const node, const bool movable){
	std::vector<Node*>& nodes = movable ? movableNodes : stationaryNodes;
	const std::vector<Node*>::iterator iter = std::find(nodes.begin(), nodes.end(), node);

	if(iter != nodes.end()){
		return assert(false && "Node could not be found!");
	}

	nodes.erase(iter);
}

void Region::ClearMovableAndDeactivateChildren(){
	if(topLeft){
		topLeft->movableNodes.clear();
		topLeft->active = false;
		topLeft->ClearMovableAndDeactivateChildren();
	}
	if(topRight){
		topRight->movableNodes.clear();
		topRight->active = false;
		topRight->ClearMovableAndDeactivateChildren();
	}
	if(bottomLeft){
		bottomLeft->movableNodes.clear();
		bottomLeft->active = false;
		bottomLeft->ClearMovableAndDeactivateChildren();
	}
	if(bottomRight){
		bottomRight->movableNodes.clear();
		bottomRight->active = false;
		bottomRight->ClearMovableAndDeactivateChildren();
	}
}

void Region::Partition(const bool movable){
	const std::vector<Node*>& entities = movable ? movableNodes : stationaryNodes;
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