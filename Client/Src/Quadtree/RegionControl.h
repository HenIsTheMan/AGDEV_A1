#pragma once

#include "Region.h"

class RegionControl final{
public:
	RegionControl();
	~RegionControl();

	void Update();
	void Render() const;

	void AddEntity(Entity* const& entity);
	void RemoveEntity(Entity* const& entity);
	void DeactivateAndClear();

	 const Region* FindRegion(Entity* const& entity);

	void InitRegionPool(const int& amt);
	void DestroyRegionPool();

	Region* FetchRegion();

	static void Partition();

	// Collision Check for an entity against Spatial Partition's entities
	//virtual bool CheckForCollisionWithEntity(CEntity3D* cEntity3D);

	// This view is for calculating frustum culling 
	// as we may detach the camera from the player.
	//glm::mat4 frustumCullingView;
private:
	//CFrustumCulling* cFrustumCulling;

	Region* root;
};