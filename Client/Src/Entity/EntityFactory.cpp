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
	entity->force = glm::vec3(0.0f, -3400.f, 0.0f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
	BoxCollider* const boxCollider = static_cast<BoxCollider*>(entity->collider);
	boxCollider->SetPos(entity->pos);
	boxCollider->SetScale(entity->scale);

	entity->moveSpd = 0.0f;
	entity->facingDir = glm::vec3(0.0f, 0.0f, -1.0f);
	entity->yMax = FLT_MAX;

	ActivateEntityProcedure(entity);
	return entity;
}

void EntityFactory::CreateShotgunBullet(const glm::vec3& camPos, const glm::vec3& camFront){
	Entity* const entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Bullet;
	entity->life = 5.0f;
	entity->maxLife = 5.0f;
	entity->colour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	entity->diffuseTexIndex = -1;
	entity->scale = glm::vec3(10.0f);

	entity->pos = camPos + 500.0f * camFront;
	entity->moveSpd = 4000.f;
	entity->facingDir = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-1.f, 1.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //Bullet bloom
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Sphere);
	SphereCollider* const sphereCollider = static_cast<SphereCollider*>(entity->collider);
	sphereCollider->SetPos(entity->pos);
	sphereCollider->SetRadius(entity->scale[0]);

	ActivateEntityProcedure(entity);
}

void EntityFactory::CreateScarBullet(const glm::vec3& camPos, const glm::vec3& camFront){
	Entity* const entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Bullet;
	entity->life = 5.0f;
	entity->maxLife = 5.0f;
	entity->colour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	entity->diffuseTexIndex = -1;
	entity->scale = glm::vec3(10.0f);

	entity->pos = camPos + 500.0f * camFront;
	entity->moveSpd = 2400.f;
	entity->facingDir = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-2.f, 2.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //Bullet bloom
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Sphere);
	SphereCollider* const sphereCollider = static_cast<SphereCollider*>(entity->collider);
	sphereCollider->SetPos(entity->pos);
	sphereCollider->SetRadius(entity->scale[0]);

	ActivateEntityProcedure(entity);
}

void EntityFactory::CreateSniperBullet(const glm::vec3& camPos, const glm::vec3& camFront){
	Entity* const entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Bullet;
	entity->life = 5.0f;
	entity->maxLife = 5.0f;
	entity->colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	entity->diffuseTexIndex = -1;
	entity->scale = glm::vec3(10.0f);

	entity->pos = camPos + 500.0f * camFront;
	entity->moveSpd = 8400.0f;
	entity->facingDir = camFront;
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Sphere);
	SphereCollider* const sphereCollider = static_cast<SphereCollider*>(entity->collider);
	sphereCollider->SetPos(entity->pos);
	sphereCollider->SetRadius(entity->scale[0]);

	ActivateEntityProcedure(entity);
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

	ActivateEntityProcedure(entity);
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

	ActivateEntityProcedure(entity);
}

const Entity* EntityFactory::CreateThinObj(const EntityCreationAttribs& attribs){
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

	ActivateEntityProcedure(entity);
	return entity;
}

const Entity* EntityFactory::CreateEnemyBody(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(true);

	entity->type = Entity::EntityType::EnemyBody;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->mass = 1.0f;

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
	BoxCollider* const boxCollider = static_cast<BoxCollider*>(entity->collider);
	boxCollider->SetPos(entity->pos);
	boxCollider->SetScale(entity->scale);

	ActivateEntityProcedure(entity);
	return entity;
}

const Entity* EntityFactory::CreateEnemyPart(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(true);

	entity->type = Entity::EntityType::EnemyPart;
	entity->life = 4.0f;
	entity->maxLife = 4.0f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->mass = 1.0f;

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
	BoxCollider* const boxCollider = static_cast<BoxCollider*>(entity->collider);
	boxCollider->SetPos(entity->pos);
	boxCollider->SetScale(entity->scale);

	ActivateEntityProcedure(entity);
	return entity;
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

	ActivateEntityProcedure(entity);
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

	entity->Reset();
	entity->movable = movable;

	return entity;
}

void EntityFactory::ActivateEntityProcedure(Entity* const entity){
	Node* const node = nodeManager->ActivateNode();

	node->SetEntity(entity);
	node->LocalTranslate(entity->GetPos());
	//node->LocalRotate(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)); //Can be better
	node->LocalScale(entity->GetScale());

	nodeManager->RetrieveRootNode()->AddChild(node);
	regionManager->RetrieveRootRegion()->AddNode(node, entity->movable);
}