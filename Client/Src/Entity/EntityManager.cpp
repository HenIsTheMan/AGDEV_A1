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
	regionControl->InitRegionPool(100);
	regionControl->ReserveStationaryEntities(999);
	regionControl->ReserveMovableEntities(50);
}

void EntityManager::Update(){
	regionControl->Update();
}

void EntityManager::Render(ShaderProg& SP) const{
	regionControl->Render(SP);
}

void EntityManager::CreateAmmo(const Entity::EntityType type, const EntityCreationAttribs& attribs){
	Entity* const& entity = FetchEntity();

	entity->type = type;
	entity->active = true;
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
	Entity* const& entity = FetchEntity();

	entity->type = Entity::EntityType::Coin;
	entity->active = true;
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
	Entity* const& entity = FetchEntity();

	entity->type = Entity::EntityType::Fire;
	entity->active = true;
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
	Entity* const& entity = FetchEntity();

	entity->type = Entity::EntityType::Enemy;
	entity->active = true;
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

Entity* const& EntityManager::FetchEntity(){
	for(Entity* const& entity: entityPool){
		if(!entity->active){
			return entity;
		}
	}

	entityPool.emplace_back(new Entity());
	(void)puts("1 entity was added to entityPool!\n");

	return entityPool.back();
}

EntityManager::EntityManager():
	entityPool(),
	regionControl(RegionControl::GetObjPtr())
{
	entityPool.resize(50);

	const size_t& mySize = entityPool.size();
	for(size_t i = 0; i < mySize; ++i){
		entityPool[i] = new Entity();
	}
}

//void Scene::UpdateCollisionBetweenEntities(Entity* const& entity1, Entity* const& entity2){ //For collision between bullets and enemies
//	const glm::vec3& displacementVec = entity2->pos - entity1->pos;
//	if(glm::dot(-displacementVec, -displacementVec) <= (entity1->scale.x + entity2->scale.x) * (entity1->scale.y + entity2->scale.y)){
//		entity1->active = false;
//		if(entity1->colour == glm::vec4(glm::vec3(.4f), .3f)){ //If shotgun bullet...
//			entity2->life -= 50.f * float(glm::length(cam.GetPos() - entity2->pos) <= 150.f);
//		} else if(entity1->colour == glm::vec4(glm::vec3(1.f), .3f)){ //If scar bullet...
//			entity2->life -= 5.f;
//		} else{ //If sniper bullet...
//			entity2->life = 0.f;
//		}
//		if(entity2->life <= 0.f){
//			entity2->active = false;
//			score += 200;
//		}
//	}
//}