#pragma once

#include <Core.h>

#include "Node.h"

class Region final{
	friend class RegionControl;
private:
	Region();
	~Region();

	void Render(ShaderProg& SP) const;

	const Region* FindRegion(Node* const node, const bool movable) const;
	Region* FetchRegion();

	void AddNode(Node* const node, const bool movable);
	void RemoveNode(Node* const node, const bool movable);

	void ClearMovableAndDeactivateChildren();
	void Partition(const bool movable);

	void ReserveStationaryNodes(const size_t& size);
	void ReserveMovableNodes(const size_t& size);

	void InitRegionPool(const size_t& size);
	void DestroyRegionPool();

	bool active;
	Region* parent;
	glm::vec2 origin;
	glm::vec2 size;

	Region* topLeft;
	Region* topRight;
	Region* bottomLeft;
	Region* bottomRight;

	std::vector<Node*> stationaryNodes;
	std::vector<Node*> movableNodes;

	static std::vector<Region*> regionPool;
};