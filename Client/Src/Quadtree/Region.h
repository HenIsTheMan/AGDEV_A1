#pragma once

#include <Core.h>

#include "../Entity/Entity.h"

class RegionControl;

class Region final{
	friend RegionControl;
private:
	Region();
	~Region();

	const Region* FindRegion(Entity* const entity, const bool movable) const;
	Region* FetchRegion();

	void AddEntity(Entity* const entity, const bool movable);
	void RemoveEntity(Entity* const entity, const bool movable);

	void ClearMovableAndDeactivateChildren();
	void Partition(const bool movable);

	void ReserveStationaryEntities(const size_t& size);
	void ReserveMovableEntities(const size_t& size);

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

	std::vector<Entity*> stationaryEntities;
	std::vector<Entity*> movableEntities;

	static std::vector<Region*> regionPool;
};