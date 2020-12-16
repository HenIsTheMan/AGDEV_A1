#pragma once

#include "Region.h"

class RegionControl final{
public:
	RegionControl();
	~RegionControl();

	const Region* FindRegion(Entity* const& entity);

	void AddEntity(Entity* const& entity);
	void RemoveEntity(Entity* const& entity);

	// Collision Check for an entity against Spatial Partition's entities
	//virtual bool CheckForCollisionWithEntity(CEntity3D* cEntity3D);

	// This view is for calculating frustum culling 
	// as we may detach the camera from the player.
	//glm::mat4 frustumCullingView;
private:
	//CFrustumCulling* cFrustumCulling;

	Region* root;
};