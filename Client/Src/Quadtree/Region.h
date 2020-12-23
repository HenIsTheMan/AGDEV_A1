#pragma once

#include <Core.h>

#include <unordered_set>

#include "Node.h"

#include "../Shared/Meshes.h"

#include "../World/Cam.h"
#include "../World/ModelStack.h"

#include "../ObjPool/ObjPool.h"

#include "../FrustumCulling/FrustumCulling.h"

class Region final{
	friend class RegionManager;
public:
	Region();
	~Region();

	void Reset();

	void GetEntitiesToRender(std::multimap<int, Entity*>& entitiesOpaque, std::multimap<int, Entity*>& entitiesNotOpaque, const Cam& cam, const FrustumCulling* const frustumCulling);
	void GetLeaves(std::vector<Region*>& leaves);

	const Region* FindRegion(Node* const node, const bool movable) const;

	void AddNode(Node* const node, const bool movable);
	void RemoveNode(Node* const node, const bool movable);

	void ClearMovableAndDeactivateChildren();
	void CheckOutOfBounds(const bool movable, std::vector<Entity*>& entitiesToRemove);
	void Partition(const bool movable);
	void VisibilityCheck(const FrustumCulling* const frustumCulling);
	void MakeSelfAndChildrenInvisible();

	///Getters
	const Region* GetParent() const;
	const std::vector<Node*>& GetStationaryNodes() const;
	const std::vector<Node*>& GetMovableNodes() const;
private:
	bool visible;

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

	void IGetEntitiesToRender(
		std::unordered_set<Entity*>& entitySetOpaque, std::unordered_set<Entity*>& entitySetNotOpaque,
		std::multimap<int, Entity*>& entitiesOpaque, std::multimap<int, Entity*>& entitiesNotOpaque,
		const Cam& cam, const FrustumCulling* const frustumCulling
	);
};