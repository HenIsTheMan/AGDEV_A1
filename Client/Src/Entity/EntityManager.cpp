#include "EntityManager.h"

extern float dt;

EntityManager::~EntityManager(){
	for(Entity*& entity: entityPool){
		if(entity){
			delete entity;
			entity = nullptr;
		}
	}

	rootNode->DestroyAllChildren();
	if(rootNode){
		delete rootNode;
		rootNode = nullptr;
	}
	regionControl->Destroy();
}

void EntityManager::Init(){
	entityPool.resize(99999);

	const size_t& mySize = entityPool.size();
	for(size_t i = 0; i < mySize; ++i){
		entityPool[i] = new Entity();
	}

	regionControl->InitRegionPool(500);
	regionControl->ReserveStationaryNodes(999);
	regionControl->ReserveMovableNodes(500);
}

void EntityManager::Update(){
	elapsedTime += dt;

	static float BT = 0.0f;
	if(Key(GLFW_KEY_L) && BT <= elapsedTime){
		shldRenderQSP = !shldRenderQSP;
		BT = elapsedTime + .5f;
	}

	regionControl->Update();
}

void EntityManager::Render(ShaderProg& SP, const Cam& cam) const{
	regionControl->RenderEntities(SP, cam);

	if(shldRenderQSP){
		regionControl->RenderQSP(SP, cam);
	}
}

void EntityManager::SetUpRegionsForStationary(){
	regionControl->SetUpRegionsForStationary();
}

void EntityManager::ActivateEntityProcedure(Entity* const entity){
	Node* const node = new Node();
	node->SetEntity(entity);
	rootNode->AddChild(node);
	regionControl->AddNode(node, entity->movable);
}

void EntityManager::DeactivateEntityProcedure(Entity* const entity){
	//regionControl->RemoveNode(rootNode->DetachChild(entity), entity->movable);
}

void EntityManager::CreatePlayer(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Player;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->vel = glm::vec3(0.f);
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);
}

void EntityManager::CreateShotgunBullet(const glm::vec3& camPos, const glm::vec3& camFront){
	Entity* const entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Bullet;
	entity->life = 5.f;
	entity->maxLife = 5.f;
	entity->colour = glm::vec4(glm::vec3(.4f), .3f);
	entity->diffuseTexIndex = -1;
	entity->scale = glm::vec3(.2f);

	entity->pos = camPos;
	entity->vel = 200.f * glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-1.f, 1.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //With bullet bloom
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);
}

void EntityManager::CreateScarBullet(const glm::vec3& camPos, const glm::vec3& camFront){
	Entity* const entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Bullet;
	entity->life = 5.f;
	entity->maxLife = 5.f;
	entity->colour = glm::vec4(glm::vec3(1.f), .3f);
	entity->diffuseTexIndex = -1;
	entity->scale = glm::vec3(.2f);

	entity->pos = camPos + 10.f * camFront;
	entity->vel = 180.f * glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-2.f, 2.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //With bullet bloom
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);
}

void EntityManager::CreateSniperBullet(const glm::vec3& camPos, const glm::vec3& camFront){
	Entity* const entity = ActivateEntity(true);

	entity->type = Entity::EntityType::Bullet;
	entity->life = 5.f;
	entity->maxLife = 5.f;
	entity->colour = glm::vec4(0.f, 0.f, 1.f, .3f);
	entity->diffuseTexIndex = -1;
	entity->scale = glm::vec3(.2f);

	entity->pos = camPos + 10.f * camFront;
	entity->vel = 500.f * camFront;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);
}

void EntityManager::CreateAmmo(const Entity::EntityType type, const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(false);

	entity->type = type;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
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
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->vel = glm::vec3(0.f);
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);
}

void EntityManager::CreateTree(const EntityCreationAttribs& attribs){
	Entity* const& entity = ActivateEntity(false);

	entity->type = Entity::EntityType::Tree;
	entity->life = 0.f;
	entity->maxLife = 0.f;
	entity->colour = attribs.colour;
	entity->diffuseTexIndex = attribs.diffuseTexIndex;
	entity->scale = attribs.scale;

	entity->pos = attribs.pos;
	entity->vel = glm::vec3(0.f);
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = ;
}

Entity* const EntityManager::ActivateEntity(const bool movable){
	for(Entity* const& entity: entityPool){
		if(!entity->active){
			entity->active = true;
			entity->movable = movable;

			ActivateEntityProcedure(entity);
			return entity;
		}
	}

	entityPool.emplace_back(new Entity());
	(void)puts("1 entity was added to entityPool!\n");

	Entity* const entity = entityPool.back();
	entity->active = true;
	entity->movable = movable;

	ActivateEntityProcedure(entity);
	return entity;
}

void EntityManager::DeactivateEntity(Entity* const& entity, const bool movable){
	entity->active = false;
	DeactivateEntityProcedure(entity);
}

EntityManager::EntityManager():
	shldRenderQSP(false),
	elapsedTime(0.0f),
	entityPool(),
	rootNode(new Node()),
	regionControl(RegionControl::GetObjPtr())
{
}