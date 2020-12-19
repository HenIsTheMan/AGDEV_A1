#include "EntityFactory.h"

EntityFactory::~EntityFactory(){
	colliderManager = nullptr; //Deleted in EntityManager
	nodeManager = nullptr; //Deleted in EntityManager
	regionManager = nullptr; //Deleted in EntityManager
	entityPool = nullptr; //Deleted in EntityManager
}

const Entity* EntityFactory::CreatePlayer(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Player;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 1.0f;
	entity->force = glm::vec3(0.0f, -400.f, 0.0f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
	BoxCollider* const boxCollider = static_cast<BoxCollider*>(entity->collider);
	boxCollider->SetPos(entity->pos);
	boxCollider->SetScale(entity->scale);

	entity->moveSpd = 0.0f;
	entity->facingDir = glm::vec3(0.0f, 0.0f, -1.0f);
	entity->yMax = FLT_MAX;

	return entity;
}

void EntityFactory::CreateShotgunBullet(const glm::vec3& camPos, const glm::vec3& camFront){
	Entity* const entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Bullet;
	entity->life = 5.f;
	entity->maxLife = 5.f;
	entity->colour = glm::vec4(glm::vec3(.4f), .3f);
	entity->diffuseTexIndex = -1;
	entity->scale = glm::vec3(.2f);

	entity->pos = camPos;
	entity->moveSpd = 200.f;
	entity->facingDir = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-1.f, 1.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //Bullet bloom
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Sphere);
}

void EntityFactory::CreateScarBullet(const glm::vec3& camPos, const glm::vec3& camFront){
	Entity* const entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Bullet;
	entity->life = 5.f;
	entity->maxLife = 5.f;
	entity->colour = glm::vec4(glm::vec3(1.f), .3f);
	entity->diffuseTexIndex = -1;
	entity->scale = glm::vec3(.2f);

	entity->pos = camPos + 10.f * camFront;
	entity->moveSpd = 180.f;
	entity->facingDir = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-2.f, 2.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //Bullet bloom
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Sphere);
}

void EntityFactory::CreateSniperBullet(const glm::vec3& camPos, const glm::vec3& camFront){
	Entity* const entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Bullet;
	entity->life = 5.f;
	entity->maxLife = 5.f;
	entity->colour = glm::vec4(0.f, 0.f, 1.f, .3f);
	entity->diffuseTexIndex = -1;
	entity->scale = glm::vec3(.2f);

	entity->pos = camPos + 10.f * camFront;
	entity->moveSpd = 500.0f;
	entity->facingDir = camFront;
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Sphere);
}

void EntityFactory::CreateCoin(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(false);

	entity->type = Entity::EntityType::Coin;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
	BoxCollider* const boxCollider = static_cast<BoxCollider*>(entity->collider);
	boxCollider->SetPos(entity->pos);
	boxCollider->SetScale(entity->scale);
}

void EntityFactory::CreateFire(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(false);

	entity->type = Entity::EntityType::Fire;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
}

void EntityFactory::CreateEnemy(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Enemy;
	entity->life = 100.f;
	entity->maxLife = 100.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box); //??
}

void EntityFactory::CreateThinObj(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(true);

	entity->type = Entity::EntityType::ThinObj;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->mass = 1.0f;

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
	BoxCollider* const boxCollider = static_cast<BoxCollider*>(entity->collider);
	boxCollider->SetPos(entity->pos);
	boxCollider->SetScale(entity->scale);
}

void EntityFactory::CreateTree(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(false);

	entity->type = Entity::EntityType::Tree;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
}

EntityFactory::EntityFactory():
	colliderManager(ColliderManager::GetObjPtr()),
	nodeManager(NodeManager::GetObjPtr()),
	regionManager(RegionManager::GetObjPtr()),
	entityPool(ObjPool<Entity>::GetObjPtr())
{
}

Entity* EntityFactory::ActivateEntity(const bool movable){
	Entity* const entity = entityPool->ActivateObj();
	entity->movable = movable;
	ActivateEntityProcedure(entity);
	return entity;
}

void EntityFactory::ActivateEntityProcedure(Entity* const entity){
	Node* const node = nodeManager->ActivateNode();
	node->SetEntity(entity);
	nodeManager->RetrieveRootNode()->AddChild(node);
	regionManager->AddNode(node, entity->movable);
}