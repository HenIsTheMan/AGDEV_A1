#pragma once

#include <Engine.h>

#include "Region.h"

class RegionManager final: public Singleton<RegionManager>{
	friend Singleton<RegionManager>;
public:
	~RegionManager();

	void Init(const size_t& inactiveSize, const size_t& activeSize);
	void Update();
	void Render(ShaderProg& SP);

	void UpdateFrustumCulling(const glm::mat4& view, const glm::mat4& projection, const float yOffset);

	Region* ActivateRegion(); //Not used elsewhere
	void DeactivateRegion(Region* const region); //Not used

	Region* RetrieveRootRegion();

	void SetUpRegionsForStationary();

	///Getter
	const FrustumCulling* GetFrustumCulling() const;
private:
	RegionManager();

	void RenderQuadtree(ShaderProg& SP);

	bool isCalledUpdateFrustumCulling;

	bool shldRenderQuadtree;
	float elapsedTime;

	ModelStack modelStack;
	Region* rootRegion;

	ObjPool<Region>* regionPool;

	FrustumCulling* frustumCulling;
};