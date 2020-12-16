#pragma once

#include <Engine.h>

#include "Region.h"

class RegionControl final: public Singleton<RegionControl>{
	friend Singleton<RegionControl>;
public:
	~RegionControl();

	void Update();
	void Render() const;

	const Region* FindRegion(Entity* const& entity, const bool movable);

	void AddEntity(Entity* const& entity, const bool movable);
	void RemoveEntity(Entity* const& entity, const bool movable);

	void ReserveStationaryEntities(const size_t& size);
	void ReserveMovableEntities(const size_t& size);

	void InitRegionPool(const size_t& size);

	// Collision Check for an entity against Spatial Partition's entities
	//virtual bool CheckForCollisionWithEntity(CEntity3D* cEntity3D);

	// This view is for calculating frustum culling 
	// as we may detach the camera from the player.
	//glm::mat4 frustumCullingView;
private:
	//CFrustumCulling* cFrustumCulling;
	RegionControl();

	Region* root;
};