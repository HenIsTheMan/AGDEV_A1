#include "Region.h"

#include <glm/gtx/norm.hpp>

Region::Region():
	visible(false),
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
	bool result = topLeft || topRight || bottomLeft || bottomRight;
	if(result){
		topLeft->GetEntitiesToUpdate(movableEntities, stationaryEntities);
		topRight->GetEntitiesToUpdate(movableEntities, stationaryEntities);
		bottomLeft->GetEntitiesToUpdate(movableEntities, stationaryEntities);
		bottomRight->GetEntitiesToUpdate(movableEntities, stationaryEntities);
	} else{
		for(int i = 0; i < stationaryNodes.size(); ++i){
			Entity* const entity = stationaryNodes[i]->RetrieveEntity();
			if(std::find(stationaryEntities.begin(), stationaryEntities.end(), entity) == stationaryEntities.end()){
				stationaryEntities.emplace_back(entity);
			}
		}

		for(int i = 0; i < movableNodes.size(); ++i){
			Entity* const entity = movableNodes[i]->RetrieveEntity();
			if(std::find(movableEntities.begin(), movableEntities.end(), entity) == movableEntities.end()){
				movableEntities.emplace_back(entity);
			}
		}
	}
}

void Region::GetEntitiesToRender(std::multimap<int, Entity*>& entitiesOpaque, std::multimap<int, Entity*>& entitiesNotOpaque, const Cam& cam, const FrustumCulling* const frustumCulling){
	if(!visible){ //Optimization
		return;
	}

	bool result = topLeft || topRight || bottomLeft || bottomRight;
	if(result){
		topLeft->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam, frustumCulling);
		topRight->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam, frustumCulling);
		bottomLeft->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam, frustumCulling);
		bottomRight->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam, frustumCulling);
	} else{
		const glm::vec3& camPos = cam.GetPos();

		for(int i = 0; i < stationaryNodes.size(); ++i){
			Entity* const entity = stationaryNodes[i]->RetrieveEntity();
			if(entity){
				const glm::vec3& entityPos = entity->GetPos();
				const glm::vec3& entityScale = entity->GetScale();

				if(!frustumCulling->ShldBeVisible(
					glm::vec3(entityPos.x - entityScale.x * 0.5f, entityPos.y - entityScale.y * 0.5f, entityPos.z - entityScale.z * 0.5f),
					glm::vec3(entityPos.x + entityScale.x * 0.5f, entityPos.y + entityScale.y * 0.5f, entityPos.z + entityScale.z * 0.5f))
				){
					continue;
				}

				switch(entity->type){
					case Entity::EntityType::Coin:
					case Entity::EntityType::Fire:
						entitiesNotOpaque.insert(std::make_pair((int)glm::length2(entity->pos - camPos), entity));
						break;
				}
			}
		}

		for(int i = 0; i < movableNodes.size(); ++i){
			Entity* const entity = movableNodes[i]->RetrieveEntity();
			if(entity){
				const glm::vec3& entityPos = entity->GetPos();
				const glm::vec3& entityScale = entity->GetScale();

				if(!frustumCulling->ShldBeVisible(
					glm::vec3(entityPos.x - entityScale.x * 0.5f, entityPos.y - entityScale.y * 0.5f, entityPos.z - entityScale.z * 0.5f),
					glm::vec3(entityPos.x + entityScale.x * 0.5f, entityPos.y + entityScale.y * 0.5f, entityPos.z + entityScale.z * 0.5f))
				){
					continue;
				}

				switch(entity->type){
					case Entity::EntityType::Bullet:
					case Entity::EntityType::Enemy:
					case Entity::EntityType::Player:
					case Entity::EntityType::ThinObj:
						entitiesOpaque.insert(std::make_pair((int)glm::length2(entity->pos - camPos), entity));
						break;
				}
			}
		}
	}
}

void Region::GetLeaves(std::vector<Region*>& leaves){
	bool isParent = false;

	if(topLeft){
		topLeft->GetLeaves(leaves);
		isParent = true;
	}
	if(topRight){
		topRight->GetLeaves(leaves);
		isParent = true;
	}
	if(bottomLeft){
		bottomLeft->GetLeaves(leaves);
		isParent = true;
	}
	if(bottomRight){
		bottomRight->GetLeaves(leaves);
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
		return;
	}

	if(topLeft != nullptr){
		topLeft->RemoveNode(node, movable);
	}
	if(topRight != nullptr){
		topRight->RemoveNode(node, movable);
	}
	if(bottomLeft != nullptr){
		bottomLeft->RemoveNode(node, movable);
	}
	if(bottomRight != nullptr){
		bottomRight->RemoveNode(node, movable);
	}

	nodes.erase(iter);
}

void Region::ClearMovableAndDeactivateChildren(){
	int emptyCount = 0;

	if(topLeft){
		topLeft->movableNodes.clear();
		if(topLeft->stationaryNodes.empty()){
			++emptyCount;
		}

		topLeft->ClearMovableAndDeactivateChildren();
	}
	if(topRight){
		topRight->movableNodes.clear();
		if(topRight->stationaryNodes.empty()){
			++emptyCount;
		}

		topRight->ClearMovableAndDeactivateChildren();
	}
	if(bottomLeft){
		bottomLeft->movableNodes.clear();
		if(bottomLeft->stationaryNodes.empty()){
			++emptyCount;
		}

		bottomLeft->ClearMovableAndDeactivateChildren();
	}
	if(bottomRight){
		bottomRight->movableNodes.clear();
		if(bottomRight->stationaryNodes.empty()){
			++emptyCount;
		}

		bottomRight->ClearMovableAndDeactivateChildren();
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
	const std::vector<Node*>& nodes = movable ? movableNodes : stationaryNodes; //Optimization
	if(movableNodes.size() + stationaryNodes.size() <= (size_t)1){ //Optimization
		return;
	}
	if(size.x <= 1.0f){
		return;
	}

	if(!topLeft){
		topLeft = regionPool->ActivateObj();
		topLeft->parent = this;
		topLeft->origin = glm::vec2(origin[0] - size[0] * 0.25f, origin[1] - size[1] * 0.25f);
		topLeft->size = glm::vec2(size[0] * 0.5f, size[1] * 0.5f);
	}

	if(!topRight){
		topRight = regionPool->ActivateObj();
		topRight->parent = this;
		topRight->origin = glm::vec2(origin[0] + size[0] * 0.25f, origin[1] - size[1] * 0.25f);
		topRight->size = topLeft->size;
	}

	if(!bottomLeft){
		bottomLeft = regionPool->ActivateObj();
		bottomLeft->parent = this;
		bottomLeft->origin = glm::vec2(origin[0] - size[0] * 0.25f, origin[1] + size[1] * 0.25f);
		bottomLeft->size = topLeft->size;
	}

	if(!bottomRight){
		bottomRight = regionPool->ActivateObj();
		bottomRight->parent = this;
		bottomRight->origin = glm::vec2(origin[0] + size[0] * 0.25f, origin[1] + size[1] * 0.25f);
		bottomRight->size = topLeft->size;
	}

	for(Node* const node: nodes){
		const Entity* const entity = node->GetEntity();

		if(entity){
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

void Region::VisibilityCheck(const FrustumCulling* const frustumCulling){
	if(!frustumCulling->ShldBeVisible(
		glm::vec3(origin[0] - size[0] * 0.5f, frustumCulling->yOffset, origin[1] - size[1] * 0.5f),
		glm::vec3(origin[0] + size[0] * 0.5f, frustumCulling->yOffset, origin[1] + size[1] * 0.5f))
	){ //Optimization
		return MakeSelfAndChildrenInvisible();
	}

	visible = true;

	if(topLeft != nullptr){
		topLeft->VisibilityCheck(frustumCulling);
	}
	if(topRight != nullptr){
		topRight->VisibilityCheck(frustumCulling);
	}
	if(bottomLeft != nullptr){
		bottomLeft->VisibilityCheck(frustumCulling);
	}
	if(bottomRight != nullptr){
		bottomRight->VisibilityCheck(frustumCulling);
	}
}

void Region::MakeSelfAndChildrenInvisible(){
	visible = false;

	if(topLeft != nullptr){
		topLeft->MakeSelfAndChildrenInvisible();
	}
	if(topRight != nullptr){
		topRight->MakeSelfAndChildrenInvisible();
	}
	if(bottomLeft != nullptr){
		bottomLeft->MakeSelfAndChildrenInvisible();
	}
	if(bottomRight != nullptr){
		bottomRight->MakeSelfAndChildrenInvisible();
	}
}