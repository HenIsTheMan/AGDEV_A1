#pragma once

#include <Engine.h>

#include "Region.h"

class RegionManager final: public Singleton<RegionManager>{
	friend Singleton<RegionManager>;
public:
	~RegionManager();

	void Init(const size_t& inactiveSize, const size_t& activeSize);
	void Update(std::vector<Entity*>& entitiesToRemove);
	void Render(ShaderProg& SP);

	void UpdateFrustumCulling(const glm::mat4& view, const glm::mat4& projection, const float yMin, const float yMax);

	Region* ActivateRegion(); //Not used elsewhere
	void DeactivateRegion(Region* const region); //Not used

	Region* RetrieveRootRegion();

	void SetUpRegionsForStationary(std::vector<Entity*>& entitiesToRemove);

	///Getter
	const FrustumCulling* GetFrustumCulling() const;
private:
	RegionManager();

	void RenderQuadtree(ShaderProg& SP);

	bool shldRenderQuadtree;
	float elapsedTime;

	ModelStack modelStack;
	Region* rootRegion;

	ObjPool<Region>* regionPool;

	FrustumCulling* frustumCulling;
};