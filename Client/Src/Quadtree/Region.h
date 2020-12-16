#pragma once

#include <Core.h>

#include "../AGDEV/Entity.h"

class Region final{
public:
	Region();
	~Region();

	const Region* FindEntity(Entity* const entity) const;

	void CalcTopRight() const;
	void CalcBottomLeft() const;

	void AddEntity(Entity* const entity);
	void RemoveEntity(Entity* const entity);

	void DeactivateAndClear();
	void Partition();

	///Getters
	const glm::vec3& GetOrigin() const;
	const glm::vec3& GetSize() const;

	///Setters
	void SetParent(Region* const parent);
	void SetOrigin(const glm::vec3& origin);
	void SetSize(const glm::vec3& size);
	void SetActive(const bool active);;
private:
	static std::vector<Region*> RegionPool;
	static Region* const& FetchRegion();

	Region* parent;
	glm::vec3 origin;
	glm::vec3 size;

	Region* UL;
	Region* UR;
	Region* DL;
	Region* DR;
	std::vector<Entity*>* entities;
	bool active;
};