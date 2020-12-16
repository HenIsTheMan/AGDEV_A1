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
		if(node){ //Deleted elsewhere
			node = nullptr;
		}
	}

	for(Node*& node: movableNodes){
		if(node){ //Deleted elsewhere
			node = nullptr;
		}
	}
}

void Region::GetEntitiesToRender(std::map<int, Entity*>& entitiesOpaque, std::map<int, Entity*>& entitiesNotOpaque, const Cam& cam){
	if(topLeft || topRight || bottomLeft || bottomRight){
		topLeft->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
		topRight->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
		bottomLeft->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
		bottomRight->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
		return;
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
			region->active = true;
			return region;
		}
	}

	Region* const region = new Region();
	region->active = true;

	regionPool.emplace_back(region);
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
		topLeft->active = false; //So region can be used elsewhere
		topLeft->ClearMovableAndDeactivateChildren();
		topLeft = nullptr; //For condition checking
	}
	if(topRight){
		topRight->movableNodes.clear();
		topRight->active = false; //So region can be used elsewhere
		topRight->ClearMovableAndDeactivateChildren();
		topRight = nullptr; //For condition checking
	}
	if(bottomLeft){
		bottomLeft->movableNodes.clear();
		bottomLeft->active = false; //So region can be used elsewhere
		bottomLeft->ClearMovableAndDeactivateChildren();
		bottomLeft = nullptr; //For condition checking
	}
	if(bottomRight){
		bottomRight->movableNodes.clear();
		bottomRight->active = false; //So region can be used elsewhere
		bottomRight->ClearMovableAndDeactivateChildren();
		bottomRight = nullptr; //For condition checking
	}
}

void Region::Partition(const bool movable){
	const std::vector<Node*>& nodes = movable ? movableNodes : stationaryNodes;
	if(nodes.size() <= (size_t)1){ //Limit amt of partitions here based on size??
		return;
	}

	if(topLeft == nullptr){
		topLeft = FetchRegion();
		topLeft->active = true;
		topLeft->parent = this;
		topLeft->origin = glm::vec2(origin[0] - size[0] * 0.25f, origin[1] - size[1] * 0.25f);
		topLeft->size = glm::vec2(size[0] * 0.5f, size[1] * 0.5f);
	}

	if(topRight == nullptr){
		topRight = FetchRegion();
		topRight->active = true;
		topRight->parent = this;
		topRight->origin = glm::vec2(origin[0] + size[0] * 0.25f, origin[1] - size[1] * 0.25f);
		topRight->size = topLeft->size;
	}

	if(bottomLeft == nullptr){
		bottomLeft = FetchRegion();
		bottomLeft->active = true;
		bottomLeft->parent = this;
		bottomLeft->origin = glm::vec2(origin[0] - size[0] * 0.25f, origin[1] + size[1] * 0.25f);
		bottomLeft->size = topLeft->size;
	}

	if(bottomRight == nullptr){
		bottomRight = FetchRegion();
		bottomRight->active = true;
		bottomRight->parent = this;
		bottomRight->origin = glm::vec2(origin[0] + size[0] * 0.25f, origin[1] + size[1] * 0.25f);
		bottomRight->size = topLeft->size;
	}

	for(Node* const node: nodes){
		const Entity* const entity = node->GetEntity();

		if(entity && entity->active
			&& entity->pos[0] + entity->scale[0] <= origin[0] + size[0] * 0.5f && entity->pos[0] - entity->scale[0] >= origin[0] - size[0] * 0.5f
			&& entity->pos[1] + entity->scale[1] <= origin[1] + size[1] * 0.5f && entity->pos[1] - entity->scale[1] >= origin[1] - size[1] * 0.5f
		){
			if(entity->pos[1] - entity->scale[1] <= origin[1]){
				if(entity->pos[0] - entity->scale[0] <= origin[0]){
					topRight->AddNode(node, entity->movable);
				}
				if(entity->pos[0] + entity->scale[0] >= origin[0]){
					topLeft->AddNode(node, entity->movable);
				}
			}
			if(entity->pos[1] + entity->scale[1] >= origin[1]){
				if(entity->pos[0] - entity->scale[0] <= origin[0]){
					bottomRight->AddNode(node, entity->movable);
				}
				if(entity->pos[0] + entity->scale[0] >= origin[0]){
					bottomLeft->AddNode(node, entity->movable);
				}
			}
		}
	}

	///Use recursion to continue forming the Quadtree
	topLeft->Partition(movable);
	topRight->Partition(movable);
	bottomLeft->Partition(movable);
	bottomRight->Partition(movable);
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