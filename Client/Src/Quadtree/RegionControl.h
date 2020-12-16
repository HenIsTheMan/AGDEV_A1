#pragma once

#include "Region.h"

class RegionControl final{
public:
	RegionControl();
	~RegionControl();

	void AddEntity(Entity* const& entity);
	void Deactivate();

	void Update();
	void Render() const;

	void InitRegionPool(const int& amt);
	void DestroyRegionPool();
private:

	std::vector<Region*> regionPool;
	Region* root;
};