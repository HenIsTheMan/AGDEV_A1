#pragma once

#include <Engine.h>

#include "Region.h"

class RegionControl final: public Singleton<RegionControl>{
	friend Singleton<RegionControl>;
public:
	~RegionControl();

	void Update();
	void Render(ShaderProg& SP) const;

	const Region* FindRegion(Entity* const& entity, const bool movable);

	void AddEntity(Entity* const& entity, const bool movable);
	void RemoveEntity(Entity* const& entity, const bool movable);

	void ReserveStationaryEntities(const size_t& size);
	void ReserveMovableEntities(const size_t& size);

	void InitRegionPool(const size_t& size);
private:
	RegionControl();

	Region* root;
};