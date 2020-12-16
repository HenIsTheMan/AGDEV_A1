#pragma once

#include "../Entity/Entity.h"
#include "../Quadtree/RegionControl.h"

class EntityManager final: public Singleton<EntityManager>{
	friend Singleton<EntityManager>;
public:
	~EntityManager();

	struct EntityCreationAttribs final{
		glm::vec3 pos;
		glm::vec3 collisionNormal;
		glm::vec3 scale;
		glm::vec4 colour;
		int diffuseTexIndex;
	};

	void Init();
	void Update();
	void Render(ShaderProg& SP) const;

	void CreateAmmo(const Entity::EntityType type, const EntityCreationAttribs& attribs);
	void CreateCoin(const EntityCreationAttribs& attribs);
	void CreateFire(const EntityCreationAttribs& attribs);
	void CreateEnemy(const EntityCreationAttribs& attribs);

	Entity* const& FetchEntity();
private:
	EntityManager();

	std::vector<Entity*> entityPool;
	RegionControl* regionControl;
};