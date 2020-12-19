#pragma once

#include <Engine.h>

#include "Region.h"

class RegionManager final: public Singleton<RegionManager>{
	friend Singleton<RegionManager>;
public:
	~RegionManager();

	void Update();
	void Render(ShaderProg& SP, const Cam& cam);

	void RenderQuadtree(ShaderProg& SP, const Cam& cam);

	void GetEntitiesToUpdate(std::vector<Entity*>& movableEntities, std::vector<Entity*>& stationaryEntities);
	void GetEntitiesToRender(std::map<int, Entity*>& entitiesOpaque, std::map<int, Entity*>& entitiesNotOpaque, const Cam& cam);

	const Region* FindRegion(Node* const& node, const bool movable);

	void AddNode(Node* const& node, const bool movable);
	void RemoveNode(Node* const& node, const bool movable);

	void InitRegionPool(const size_t& size);
private:
	RegionManager();

	bool shldRenderQuadtree;
	float elapsedTime;

	ModelStack modelStack;
	Region* rootRegion;
};