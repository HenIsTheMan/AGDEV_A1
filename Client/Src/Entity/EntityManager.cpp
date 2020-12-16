#include "EntityManager.h"

EntityManager::~EntityManager(){
	for(Entity*& entity: entityPool){
		if(entity){
			delete entity;
			entity = nullptr;
		}
	}

	regionControl->Destroy();
}

void EntityManager::Init(){
	entityPool.resize(99999);

	const size_t& mySize = entityPool.size();
	for(size_t i = 0; i < mySize; ++i){
		entityPool[i] = new Entity();
	}

	regionControl->InitRegionPool(100);
	regionControl->ReserveStationaryEntities(999);
	regionControl->ReserveMovableEntities(500);
}

void EntityManager::Update(){
	regionControl->Update();
}

void EntityManager::Render(ShaderProg& SP) const{
	regionControl->Render(SP);
}

void EntityManager::CreateAmmo(const Entity::EntityType type, const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(false);

	entity->type = type;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->collisionNormal = attribs.collisionNormal;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->vel = glm::vec3(0.f);
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);
}

void EntityManager::CreateCoin(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(false);

	entity->type = Entity::EntityType::Coin;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->collisionNormal = attribs.collisionNormal;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->vel = glm::vec3(0.f);
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);
}

void EntityManager::CreateFire(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(false);

	entity->type = Entity::EntityType::Fire;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->collisionNormal = attribs.collisionNormal;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->vel = glm::vec3(0.f);
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);
}

void EntityManager::CreateEnemy(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Enemy;
	entity->life = 100.f;
	entity->maxLife = 100.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->collisionNormal = attribs.collisionNormal;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->vel = glm::vec3(0.f);
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);
}

Entity* const EntityManager::ActivateEntity(const bool movable){
	for(Entity* const& entity: entityPool){
		if(!entity->active){
			entity->active = true;
			entity->movable = movable;
			return entity;
		}
	}

	entityPool.emplace_back(new Entity());
	(void)puts("1 entity was added to entityPool!\n");

	Entity* const entity = entityPool.back();
	entity->active = true;
	return entity;
}

void DeactivateEntity(const bool movable){
}

EntityManager::EntityManager():
	entityPool(),
	regionControl(RegionControl::GetObjPtr())
{
}