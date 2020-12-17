#pragma once

#include <Core.h>

#include "Node.h"

#include "../Shared/Meshes.h"

#include "../World/Cam.h"
#include "../World/ModelStack.h"

class Region final{
	friend class RegionControl;
private:
	Region();
	~Region();

	void GetEntitiesToRender(std::map<int, Entity*>& entitiesOpaque, std::map<int, Entity*>& entitiesNotOpaque, const Cam& cam);
	void GetLeaves(ShaderProg& SP, std::vector<Region*>& leaves);

	const Region* FindRegion(Node* const node, const bool movable) const;
	Region* FetchRegion();

	void AddNode(Node* const node, const bool movable);
	void RemoveNode(Node* const node, const bool movable);

	void ClearMovableAndDeactivateChildren();
	void Partition(const bool movable);

	void InitRegionPool(const size_t& size);
	void DestroyRegionPool();

	bool active;
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

	static std::vector<Region*> regionPool;
};