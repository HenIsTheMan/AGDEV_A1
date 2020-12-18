#include "EntityManager.h"

#include "EntityUpdate.hpp"

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
	colliderManager->Destroy();
}

void EntityManager::Init(){
	entityPool.resize(9999);

	const size_t& mySize = entityPool.size();
	for(size_t i = 0; i < mySize; ++i){
		entityPool[i] = new Entity();
	}

	colliderManager->Init(9999, 9999);

	regionControl->InitRegionPool(500);
	regionControl->ReserveStationaryNodes(999);
	regionControl->ReserveMovableNodes(500);
}

void EntityManager::Update(){
	regionControl->Update();

	std::vector<Entity*> movableEntities;
	std::vector<Entity*> stationaryEntities;
	regionControl->GetEntitiesToUpdate(movableEntities, stationaryEntities);

	for(Entity* const movableEntity: movableEntities){
		switch(movableEntity->type){
			case Entity::EntityType::Player:
				UpdatePlayerHoriz(movableEntity);
				UpdatePlayerVert(movableEntity);

				movableEntity->vel += movableEntity->force / movableEntity->mass * dt;
				movableEntity->pos += movableEntity->vel * dt;

				if(movableEntity->pos.y < movableEntity->yMin){
					IsAirborneWrapper::isAirborne = false;
				}

				movableEntity->yMin = terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(
					movableEntity->pos.x / terrainXScale,
					movableEntity->pos.z / terrainZScale, false
				) + movableEntity->scale.y * 0.5f;

				movableEntity->pos.x = std::min(movableEntity->xMax, std::max(movableEntity->xMin, movableEntity->pos.x));
				movableEntity->pos.y = std::min(movableEntity->yMax, std::max(movableEntity->yMin, movableEntity->pos.y));
				movableEntity->pos.z = std::min(movableEntity->zMax, std::max(movableEntity->zMin, movableEntity->pos.z));

				break;
		}
	}

	//Update dragon??
	//for(Entity* const stationaryEntity: stationaryEntities){
	//}
}

void EntityManager::Render(ShaderProg& SP, const Cam& cam){
	regionControl->Render(SP, cam);

	SP.Set1i("noNormals", 1);
	SP.Set1i("useCustomColour", 1);
	SP.Set1i("useCustomDiffuseTexIndex", 1);

	///Use std::map so render order is correct
	std::map<int, Entity*> entitiesOpaque;
	std::map<int, Entity*> entitiesNotOpaque;
	regionControl->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);

	///Render opaque entities 1st
	for(std::map<int, Entity*>::reverse_iterator iter = entitiesOpaque.rbegin(); iter != entitiesOpaque.rend(); ++iter){
		Entity* const& entity = iter->second;

		SP.Set4fv("customColour", entity->colour);
		SP.Set1i("customDiffuseTexIndex", entity->diffuseTexIndex);

		switch(entity->type){
			case Entity::EntityType::Bullet:
			case Entity::EntityType::Enemy:
				modelStack.PushModel({
					modelStack.Translate(entity->pos),
					modelStack.Scale(entity->scale),
				});
					Meshes::meshes[(int)MeshType::Sphere]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::Sphere]->Render(SP);
				break;
			case Entity::EntityType::ShotgunAmmo:
			case Entity::EntityType::ScarAmmo:
			case Entity::EntityType::SniperAmmo:
			case Entity::EntityType::Player:
				modelStack.PushModel({
					modelStack.Translate(entity->pos),
					modelStack.Scale(entity->scale),
				});
					Meshes::meshes[(int)MeshType::Cube]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::Cube]->Render(SP);
				break;
		}
		modelStack.PopModel();
	}

	SP.Set1i("useCustomColour", 0);
	SP.Set1i("useCustomDiffuseTexIndex", 0);

	///Then render non-opaque entities
	for(std::map<int, Entity*>::reverse_iterator iter = entitiesNotOpaque.rbegin(); iter != entitiesNotOpaque.rend(); ++iter){
		Entity* const& entity = iter->second;

		switch(entity->type){
			case Entity::EntityType::Coin:
				modelStack.PushModel({
					modelStack.Translate(entity->pos),
					modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, glm::degrees(atan2(cam.GetPos().x - entity->pos.x, cam.GetPos().z - entity->pos.z)))),
					modelStack.Scale(entity->scale),
				});
					Meshes::meshes[(int)MeshType::CoinSpriteAni]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::CoinSpriteAni]->Render(SP);
				break;
			case Entity::EntityType::Fire:
				modelStack.PushModel({
					modelStack.Translate(entity->pos + glm::vec3(0.f, entity->scale.y / 2.f, 0.f)),
					modelStack.Rotate(glm::vec4(0.f, 1.f, 0.f, glm::degrees(atan2(cam.GetPos().x - entity->pos.x, cam.GetPos().z - entity->pos.z)))),
					modelStack.Scale(glm::vec3(entity->scale.x, entity->scale.y * 2.f, entity->scale.z)),
				});
					Meshes::meshes[(int)MeshType::FireSpriteAni]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::FireSpriteAni]->Render(SP);
				break;
		}
		modelStack.PopModel();
	}

	SP.Set1i("noNormals", 0);
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
	entity->moveSpd = 200.f;
	entity->facingDir = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-1.f, 1.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //Bullet bloom
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Sphere);
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
	entity->moveSpd = 180.f;
	entity->facingDir = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-2.f, 2.f)), {0.f, 1.f, 0.f}) * glm::vec4(camFront, 0.f)); //Bullet bloom
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Sphere);
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
	entity->moveSpd = 500.0f;
	entity->facingDir = camFront;
	entity->vel = entity->moveSpd * entity->facingDir;
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Sphere);
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
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
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
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
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
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
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
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box); //??
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
	entity->mass = 5.f;
	entity->force = glm::vec3(0.f);

	entity->collider = colliderManager->ActivateCollider(ColliderType::Box);
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

	Collider* const collider = entity->collider;
	if(collider != nullptr){
		colliderManager->DeactivateCollider(collider);
	}

	DeactivateEntityProcedure(entity);
}

EntityManager::EntityManager():
	entityPool(),
	rootNode(new Node()),
	regionControl(RegionControl::GetObjPtr()),
	colliderManager(ColliderManager::GetObjPtr()),
	modelStack()
{
}