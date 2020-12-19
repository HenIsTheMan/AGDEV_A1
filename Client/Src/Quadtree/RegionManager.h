#pragma once

#include <Engine.h>

#include "Region.h"

#include "../ObjPool/ObjPool.h"

class RegionManager final: public Singleton<RegionManager>{
	friend Singleton<RegionManager>;
public:
	~RegionManager();

	void Init(const size_t& inactiveSize, const size_t& activeSize);
	void Update();
	void Render(ShaderProg& SP, const Cam& cam);

	Region* RetrieveRootRegion();
private:
	RegionManager();

	void RenderQuadtree(ShaderProg& SP, const Cam& cam);

	bool shldRenderQuadtree;
	float elapsedTime;

	ModelStack modelStack;
	Region* rootRegion;

	ObjPool<Region>* regionPool;
};