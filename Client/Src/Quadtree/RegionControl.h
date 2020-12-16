#pragma once

#include <Engine.h>

#include "Region.h"

class RegionControl final: public Singleton<RegionControl>{
	friend Singleton<RegionControl>;
public:
	~RegionControl();

	void Update();
	void Render(ShaderProg& SP, const Cam& cam) const;

	const Region* FindRegion(Node* const& node, const bool movable);

	void AddNode(Node* const& node, const bool movable);
	void RemoveNode(Node* const& node, const bool movable);

	void ReserveStationaryNodes(const size_t& size);
	void ReserveMovableNodes(const size_t& size);

	void InitRegionPool(const size_t& size);

	void SetUpRegionsForStationary();
private:
	RegionControl();

	Region* rootRegion;
};