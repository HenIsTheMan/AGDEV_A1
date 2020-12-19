#include "Region.h"

Region::Region():
	parent(nullptr),
	origin(glm::vec2()),
	size(glm::vec2()),
	color(glm::vec4(PseudorandMinMax(0.0f, 1.0f), PseudorandMinMax(0.0f, 1.0f), PseudorandMinMax(0.0f, 1.0f), 1.0f)),
	topLeft(nullptr),
	topRight(nullptr),
	bottomLeft(nullptr),
	bottomRight(nullptr),
	stationaryNodes(),
	movableNodes(),
	regionPool(ObjPool<Region>::GetObjPtr())
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

	regionPool = nullptr; //Deleted in RegionManager
}

void Region::GetEntitiesToUpdate(std::vector<Entity*>& movableEntities, std::vector<Entity*>& stationaryEntities){
	if(topLeft || topRight || bottomLeft || bottomRight){
		topLeft->GetEntitiesToUpdate(movableEntities, stationaryEntities);
		topRight->GetEntitiesToUpdate(movableEntities, stationaryEntities);
		bottomLeft->GetEntitiesToUpdate(movableEntities, stationaryEntities);
		bottomRight->GetEntitiesToUpdate(movableEntities, stationaryEntities);
		return;
	}

	for(int i = 0; i < stationaryNodes.size(); ++i){
		stationaryEntities.emplace_back(stationaryNodes[i]->RetrieveEntity());
	}

	for(int i = 0; i < movableNodes.size(); ++i){
		movableEntities.emplace_back(movableNodes[i]->RetrieveEntity());
	}
}

void Region::GetEntitiesToRender(std::multimap<int, Entity*>& entitiesOpaque, std::multimap<int, Entity*>& entitiesNotOpaque, const Cam& cam){
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
		if(entity){
			glm::vec3 displacementVec = entity->pos - camPos;
			if(glm::dot(displacementVec, camFront) > 0.0f){
				switch(entity->type){
					case Entity::EntityType::Coin:
					case Entity::EntityType::Fire:
						entitiesNotOpaque.insert(std::make_pair((int)glm::dot(displacementVec, displacementVec), entity));
						break;
				}
			}
		}
	}

	for(int i = 0; i < movableNodes.size(); ++i){
		Entity* const entity = movableNodes[i]->RetrieveEntity();
		if(entity){
			glm::vec3 displacementVec = entity->pos - camPos;
			if(glm::dot(displacementVec, camFront) > 0.0f){
				switch(entity->type){
					case Entity::EntityType::Bullet:
					case Entity::EntityType::Enemy:
					case Entity::EntityType::Player:
					case Entity::EntityType::ThinObj:
						entitiesOpaque.insert(std::make_pair((int)glm::dot(displacementVec, displacementVec), entity));
						break;
				}
			}
		}
	}
}

void Region::GetLeaves(ShaderProg& SP, std::vector<Region*>& leaves){
	bool isParent = false;

	if(topLeft){
		topLeft->GetLeaves(SP, leaves);
		isParent = true;
	}
	if(topRight){
		topRight->GetLeaves(SP, leaves);
		isParent = true;
	}
	if(bottomLeft){
		bottomLeft->GetLeaves(SP, leaves);
		isParent = true;
	}
	if(bottomRight){
		bottomRight->GetLeaves(SP, leaves);
		isParent = true;
	}

	if(isParent){
		return;
	}
	leaves.emplace_back(this);
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

void Region::AddNode(Node* const node, const bool movable){
	(movable ? movableNodes : stationaryNodes).emplace_back(node);
}

void Region::RemoveNode(Node* const node, const bool movable){
	std::vector<Node*>& nodes = movable ? movableNodes : stationaryNodes;
	const std::vector<Node*>::iterator iter = std::find(nodes.begin(), nodes.end(), node);

	if(iter == nodes.end()){
		return assert(false && "Node could not be found!");
	}

	nodes.erase(iter);
}




void Region::ClearMovableAndDeactivateChildren(){
	int emptyCount = 0;

	if(topLeft){
		topLeft->movableNodes.clear();
		topLeft->ClearMovableAndDeactivateChildren();

		if(topLeft->stationaryNodes.empty()){
			++emptyCount;
		}
	}
	if(topRight){
		topRight->movableNodes.clear();
		topRight->ClearMovableAndDeactivateChildren();

		if(topRight->stationaryNodes.empty()){
			++emptyCount;
		}
	}
	if(bottomLeft){
		bottomLeft->movableNodes.clear();
		bottomLeft->ClearMovableAndDeactivateChildren();

		if(bottomLeft->stationaryNodes.empty()){
			++emptyCount;
		}
	}
	if(bottomRight){
		bottomRight->movableNodes.clear();
		bottomRight->ClearMovableAndDeactivateChildren();

		if(bottomRight->stationaryNodes.empty()){
			++emptyCount;
		}
	}

	if(emptyCount == 4){
		regionPool->DeactivateObj(topLeft);
		topLeft = nullptr;

		regionPool->DeactivateObj(topRight);
		topRight = nullptr;

		regionPool->DeactivateObj(bottomLeft);
		bottomLeft = nullptr;

		regionPool->DeactivateObj(bottomRight);
		bottomRight = nullptr;
	}
}

void Region::Partition(const bool movable){
	const std::vector<Node*>& nodes = movable ? movableNodes : stationaryNodes;
	if(movableNodes.size() + stationaryNodes.size() <= (size_t)1){
		return;
	}

	if(!(topLeft == nullptr && topRight == nullptr && bottomLeft == nullptr && bottomRight == nullptr)
		&& !(topLeft != nullptr && topRight != nullptr && bottomLeft != nullptr && bottomRight != nullptr)){
		return assert(false && "QSP is wrong!");
	}

	topLeft = regionPool->ActivateObj();
	topLeft->parent = this;
	topLeft->origin = glm::vec2(origin[0] - size[0] * 0.25f, origin[1] - size[1] * 0.25f);
	topLeft->size = glm::vec2(size[0] * 0.5f, size[1] * 0.5f);

	topRight = regionPool->ActivateObj();
	topRight->parent = this;
	topRight->origin = glm::vec2(origin[0] + size[0] * 0.25f, origin[1] - size[1] * 0.25f);
	topRight->size = topLeft->size;

	bottomLeft = regionPool->ActivateObj();
	bottomLeft->parent = this;
	bottomLeft->origin = glm::vec2(origin[0] - size[0] * 0.25f, origin[1] + size[1] * 0.25f);
	bottomLeft->size = topLeft->size;

	bottomRight = regionPool->ActivateObj();
	bottomRight->parent = this;
	bottomRight->origin = glm::vec2(origin[0] + size[0] * 0.25f, origin[1] + size[1] * 0.25f);
	bottomRight->size = topLeft->size;

	for(Node* const node: nodes){
		const Entity* const entity = node->GetEntity();

		if(entity
			&& entity->pos.x + entity->scale.x * 0.5f <= origin[0] + size[0] * 0.5f && entity->pos.x - entity->scale.x * 0.5f >= origin[0] - size[0] * 0.5f
			&& entity->pos.z + entity->scale.z * 0.5f <= origin[1] + size[1] * 0.5f && entity->pos.z - entity->scale.z * 0.5f >= origin[1] - size[1] * 0.5f
		){
			if(entity->pos.z - entity->scale.z * 0.5f <= origin[1]){
				if(entity->pos.x - entity->scale.x * 0.5f <= origin[0]){
					topLeft->AddNode(node, entity->movable);
				}
				if(entity->pos.x + entity->scale.x * 0.5f >= origin[0]){
					topRight->AddNode(node, entity->movable);
				}
			}
			if(entity->pos.z + entity->scale.z * 0.5f >= origin[1]){
				if(entity->pos.x - entity->scale.x * 0.5f <= origin[0]){
					bottomLeft->AddNode(node, entity->movable);
				}
				if(entity->pos.x + entity->scale.x * 0.5f >= origin[0]){
					bottomRight->AddNode(node, entity->movable);
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