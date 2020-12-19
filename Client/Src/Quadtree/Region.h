#pragma once

#include <Core.h>

#include "Node.h"

#include "../Shared/Meshes.h"

#include "../World/Cam.h"
#include "../World/ModelStack.h"

#include "../ObjPool/ObjPool.h"

class Region final{
	friend class RegionManager;
public:
	Region();
	~Region();

	void GetEntitiesToUpdate(std::vector<Entity*>& movableEntities, std::vector<Entity*>& stationaryEntities);
	void GetEntitiesToRender(std::multimap<int, Entity*>& entitiesOpaque, std::multimap<int, Entity*>& entitiesNotOpaque, const Cam& cam);
	void GetLeaves(ShaderProg& SP, std::vector<Region*>& leaves);

	const Region* FindRegion(Node* const node, const bool movable) const;

	void AddNode(Node* const node, const bool movable);
	void RemoveNode(Node* const node, const bool movable);

	void ClearMovableAndDeactivateChildren();
	void Partition(const bool movable);
private:
	Region* parent;
	glm::vec2 origin;
	glm::vec2 size;
	glm::vec4 color;

	Region* topLeft;
	Region* topRight;
	Region* bottomLeft;
	Region* bottomRight;

	std::vector<Node*> stationaryNodes;
	std::vector<Node*> movableNodes;

	ObjPool<Region>* regionPool;
};