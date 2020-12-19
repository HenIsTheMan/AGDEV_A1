#include "EntityManager.h"

#include "EntityUpdate.hpp"

#include "../Shared/Easing.hpp"

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

	regionManager->Destroy();
	colliderManager->Destroy();
}

void EntityManager::Init(){
	const size_t entityPoolSize = 9999;

	entityPool.reserve(entityPoolSize);
	for(size_t i = 0; i < entityPoolSize; ++i){
		entityPool.emplace_back(new Entity());
	}

	colliderManager->InitBoxColliderPool(entityPoolSize, entityPoolSize);
	colliderManager->InitSphereColliderPool(entityPoolSize, entityPoolSize);

	regionManager->InitRegionPool(entityPoolSize);
}

void EntityManager::Update(){
	elapsedTime += dt;

	static bool isPressedC = false;
	if(!isPressedC && Key(GLFW_KEY_C)){
		shldRenderColliders = !shldRenderColliders;
		isPressedC = true;
	} else if(isPressedC && !Key(GLFW_KEY_C)){
		isPressedC = false;
	}

	regionManager->Update();

	std::vector<Entity*> movableEntities;
	std::vector<Entity*> stationaryEntities;
	regionManager->GetEntitiesToUpdate(movableEntities, stationaryEntities);

	for(Entity* const movableEntity: movableEntities){
		switch(movableEntity->type){
			case Entity::EntityType::Player: {
				UpdatePlayerHoriz(movableEntity);
				UpdatePlayerVert(movableEntity);

				const glm::vec3 prevPos = movableEntity->pos;
				movableEntity->vel += movableEntity->force / movableEntity->mass * dt;
				movableEntity->pos += movableEntity->vel * dt;

				if(movableEntity->pos.y < movableEntity->yMin){
					IsAirborneWrapper::isAirborne = false;
				}

				movableEntity->yMin = terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(
					movableEntity->pos.x / terrainXScale,
					movableEntity->pos.z / terrainZScale,
					false
				) + movableEntity->scale.y * 0.5f;

				movableEntity->pos.x = std::min(movableEntity->xMax, std::max(movableEntity->xMin, movableEntity->pos.x));
				movableEntity->pos.y = std::min(movableEntity->yMax, std::max(movableEntity->yMin, movableEntity->pos.y));
				movableEntity->pos.z = std::min(movableEntity->zMax, std::max(movableEntity->zMin, movableEntity->pos.z));

				if(movableEntity->collider != nullptr){
					static_cast<BoxCollider*>(movableEntity->collider)->SetPos(movableEntity->pos);
				}

				break;
			}
			case Entity::EntityType::ThinObj:
				//movableEntity->pos.x += EaseInOutCubic((sinf(elapsedTime * 2.0f) + 1.0f) * 0.5f);
				movableEntity->pos.x += sinf(elapsedTime * 2.0f);

				if(movableEntity->collider != nullptr){
					static_cast<BoxCollider*>(movableEntity->collider)->SetPos(movableEntity->pos);
				}

				break;
		}
	}

	//Update dragon??
	//for(Entity* const stationaryEntity: stationaryEntities){
	//}
}

void EntityManager::Render(ShaderProg& SP, const Cam& cam){
	regionManager->Render(SP, cam);

	SP.Set1i("noNormals", 1);
	SP.Set1i("useCustomColour", 1);
	SP.Set1i("useCustomDiffuseTexIndex", 1);

	///Use std::map so render order is correct
	std::map<int, Entity*> entitiesOpaque;
	std::map<int, Entity*> entitiesNotOpaque;
	regionManager->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);

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
			case Entity::EntityType::ThinObj:
				modelStack.PushModel({
					modelStack.Translate(entity->pos),
					modelStack.Scale(entity->scale),
				});
					Meshes::meshes[(int)MeshType::Cube]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::Cube]->Render(SP);
				break;
		}
		modelStack.PopModel();

		if(shldRenderColliders && entity->collider != nullptr){
			SP.Set4fv("customColour", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

			switch(entity->collider->GetType()){
				case ColliderType::Box: {
					BoxCollider* const boxCollider = static_cast<BoxCollider*>(entity->collider);

					modelStack.PushModel({
						modelStack.Translate(boxCollider->GetPos()),
						modelStack.Scale(boxCollider->GetScale()),
					});
						Meshes::meshes[(int)MeshType::CubeBB]->SetModel(modelStack.GetTopModel());
						Meshes::meshes[(int)MeshType::CubeBB]->Render(SP);
					break;
				}
				case ColliderType::Sphere:
					break;
			}
			modelStack.PopModel();
		}
	}

	SP.Set1i("useCustomDiffuseTexIndex", 0);

	///Then render non-opaque entities
	for(std::map<int, Entity*>::reverse_iterator iter = entitiesNotOpaque.rbegin(); iter != entitiesNotOpaque.rend(); ++iter){
		Entity* const& entity = iter->second;
		SP.Set1i("useCustomColour", 0);

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

		if(shldRenderColliders && entity->collider != nullptr){
			SP.Set1i("useCustomColour", 1);
			SP.Set4fv("customColour", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

			switch(entity->collider->GetType()){
				case ColliderType::Box: {
					BoxCollider* const boxCollider = static_cast<BoxCollider*>(entity->collider);

					modelStack.PushModel({
						modelStack.Translate(boxCollider->GetPos()),
						modelStack.Scale(boxCollider->GetScale()),
					});
						Meshes::meshes[(int)MeshType::CubeBB]->SetModel(modelStack.GetTopModel());
						Meshes::meshes[(int)MeshType::CubeBB]->Render(SP);
					break;
				}
				case ColliderType::Sphere:
					break;
			}
			modelStack.PopModel();
		}
	}

	SP.Set1i("noNormals", 0);
	SP.Set1i("useCustomColour", 0);
}

void EntityManager::ActivateEntityProcedure(Entity* const entity){
	Node* const node = new Node();
	node->SetEntity(entity);
	rootNode->AddChild(node);
	regionManager->AddNode(node, entity->movable);
}

void EntityManager::DeactivateEntityProcedure(Entity* const entity){
	//regionManager->RemoveNode(rootNode->DetachChild(entity), entity->movable);
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
	shldRenderColliders(false),
	elapsedTime(0.0f),
	entityPool(),
	rootNode(new Node()),
	regionManager(RegionManager::GetObjPtr()),
	entityFactory(EntityFactory::GetObjPtr()),
	colliderManager(ColliderManager::GetObjPtr()),
	modelStack()
{
}