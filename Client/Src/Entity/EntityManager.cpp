#include "EntityManager.h"

#include "EntityUpdate.hpp"

#include "../Shared/Easing.hpp"

#include "../Collision/Collision.h"

extern float dt;

EntityManager::~EntityManager(){
	if(entityFactory != nullptr){
		entityFactory->Destroy();
		entityFactory = nullptr;
	}

	if(colliderManager != nullptr){
		colliderManager->Destroy();
		colliderManager = nullptr;
	}

	if(nodeManager != nullptr){
		nodeManager->Destroy();
		nodeManager = nullptr;
	}

	if(regionManager != nullptr){
		regionManager->Destroy();
		regionManager = nullptr;
	}

	if(entityPool != nullptr){
		entityPool->Destroy();
		entityPool = nullptr;
	}
}

void EntityManager::Init(){
	const size_t entityPoolSize = 40000;

	colliderManager->InitBoxColliderPool(entityPoolSize, entityPoolSize);
	colliderManager->InitSphereColliderPool(entityPoolSize, entityPoolSize);

	nodeManager->Init(entityPoolSize, entityPoolSize);

	regionManager->Init(entityPoolSize, entityPoolSize);

	entityPool->Init(entityPoolSize, entityPoolSize);
}

void EntityManager::Update(const Cam& cam){
	elapsedTime += dt;

	static bool isPressedC = false;
	if(!isPressedC && Key(GLFW_KEY_C)){
		shldRenderColliders = !shldRenderColliders;
		isPressedC = true;
	} else if(isPressedC && !Key(GLFW_KEY_C)){
		isPressedC = false;
	}

	std::vector<Entity*> entitiesToRemove;
	regionManager->Update(entitiesToRemove);
	for(Entity* const entity: entitiesToRemove){
		DeactivateEntity(entity);
	}
	entitiesToRemove.clear();

	std::vector<Node*> movableNodes = regionManager->RetrieveRootRegion()->GetMovableNodes();
	for(Node*& movableNode: movableNodes){
		if(movableNode){
			Entity* const movableEntity = movableNode->RetrieveEntity();
			movableEntity->prevPos = movableEntity->pos;

			switch(movableEntity->type){
				case Entity::EntityType::Player: {
					UpdatePlayerHoriz(movableEntity, isCamDetached);
					UpdatePlayerVert(movableEntity);

					movableEntity->vel += movableEntity->force / movableEntity->mass * dt;
					movableNode->LocalTranslate(movableEntity->vel * dt);

					//if(movableEntity->pos.y < movableEntity->yMin){
					//	IsAirborneWrapper::isAirborne = false;
					//}

					movableEntity->yMin = terrainYScale * static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(
						movableEntity->pos.x / terrainXScale,
						movableEntity->pos.z / terrainZScale,
						false
					) + movableEntity->scale.y;

					break;
				}
				case Entity::EntityType::ThinObj: {
					const float startX = 500.0f;
					const float endX = -500.0f;
					float t = EaseInOutCubic(sin(elapsedTime) * 0.5f + 0.5f);
					t *= t;
					movableEntity->pos.x = (1 - t) * startX + t * endX;

					movableNode->SetLocalTranslation(movableEntity->pos);

					break;
				}
				case Entity::EntityType::Bullet: {
					movableEntity->life -= dt;

					if(movableEntity->life <= 0.0f){
						if(std::find(entitiesToRemove.begin(), entitiesToRemove.end(), movableEntity) == entitiesToRemove.end()){
							entitiesToRemove.emplace_back(movableEntity);
							movableNode = nullptr;
						}
						continue;
					}

					movableEntity->vel += movableEntity->force / movableEntity->mass * dt;
					movableNode->LocalTranslate(movableEntity->vel * dt);

					break;
				}
				case Entity::EntityType::EnemyBody: {
					movableNode->LocalTranslate(glm::vec3(40.0f, 0.0f, 0.0f) * dt);
					movableNode->LocalScale(glm::vec3(20.0f, 20.0f, 0.0f) * dt);
					break;
				}
				//case Entity::EntityType::EnemyPart: {
				//	const float startX = 200.0f;
				//	const float endX = -200.0f;
				//	float t = EaseInOutCubic(sin(elapsedTime) * 0.5f + 0.5f);
				//	t *= t;

				//	movableNode->LocalTranslate(glm::vec3((1 - t) * startX + t * endX, 0.0f, 0.0f));

				//	if(movableEntity->collider != nullptr){
				//		static_cast<BoxCollider*>(movableEntity->collider)->SetPos(movableEntity->pos);
				//	}
				//}
			}
		}
	}

	const size_t size = movableNodes.size();
	for(size_t i = 0; i < size; ++i){
		Node*& movableNode0 = movableNodes[i];
		if(movableNode0 == nullptr){
			continue;
		}

		for(size_t j = i + 1; j < size; ++j){
			Node*& movableNode1 = movableNodes[j];
			if(movableNode1 == nullptr){
				continue;
			}

			Entity* const entity0 = movableNode0->RetrieveEntity();
			Entity* const entity1 = movableNode1->RetrieveEntity();

			if(entity0->type != entity1->type){
				if(entity0->type == Entity::EntityType::Bullet){
					if(Collision::DetectCollision(entity0, entity1)){
						switch(entity1->type){
							case Entity::EntityType::ThinObj:
								entity1->colour = glm::vec4(PseudorandMinMax(0.0f, 1.0f), PseudorandMinMax(0.0f, 1.0f), PseudorandMinMax(0.0f, 1.0f), 1.0f);
								break;
						}

						if(std::find(entitiesToRemove.begin(), entitiesToRemove.end(), entity0) == entitiesToRemove.end()){
							entitiesToRemove.emplace_back(entity0);
							movableNode0 = nullptr;
							break;
						}
					}
				} else if(entity1->type == Entity::EntityType::Bullet){
					if(Collision::DetectCollision(entity1, entity0)){
						switch(entity0->type){
							case Entity::EntityType::ThinObj:
								entity0->colour = glm::vec4(PseudorandMinMax(0.0f, 1.0f), PseudorandMinMax(0.0f, 1.0f), PseudorandMinMax(0.0f, 1.0f), 1.0f);
								break;
						}

						if(std::find(entitiesToRemove.begin(), entitiesToRemove.end(), entity1) == entitiesToRemove.end()){
							entitiesToRemove.emplace_back(entity1);
							movableNode1 = nullptr;
							break;
						}
					}
				}
			}
		}
	}

	for(Entity* const entity: entitiesToRemove){
		DeactivateEntity(entity);
	}
	entitiesToRemove.clear();

	nodeManager->Update();
}

void EntityManager::Render(ShaderProg& SP, const Cam& cam){
	regionManager->Render(SP);

	SP.Set1i("noNormals", 1);
	SP.Set1i("useCustomColour", 1);
	SP.Set1i("useCustomDiffuseTexIndex", 1);

	///2 std::multimap for opaque entities and non-opaque entities
	///std::multimap so each elment will be sorted by its key
	///std::multimap instead of std::map to allow for duplicate keys
	std::multimap<int, Entity*> entitiesOpaque;
	std::multimap<int, Entity*> entitiesNotOpaque;
	regionManager->RetrieveRootRegion()->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam, regionManager->GetFrustumCulling());

	///Render opaque entities 1st
	for(std::multimap<int, Entity*>::reverse_iterator iter = entitiesOpaque.rbegin(); iter != entitiesOpaque.rend(); ++iter){
		Entity* const& entity = iter->second;

		SP.Set4fv("customColour", entity->colour);
		SP.Set1i("customDiffuseTexIndex", entity->diffuseTexIndex);

		switch(entity->type){
			case Entity::EntityType::Bullet:
				modelStack.PushModel({
					modelStack.Translate(entity->pos),
					modelStack.Scale(entity->scale),
				});
					Meshes::meshes[(int)MeshType::Sphere]->SetModel(modelStack.GetTopModel());
					Meshes::meshes[(int)MeshType::Sphere]->Render(SP);
				break;
			case Entity::EntityType::Player:
			case Entity::EntityType::ThinObj:
			case Entity::EntityType::EnemyBody:
			case Entity::EntityType::EnemyPart:
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
						Meshes::meshes[(int)MeshType::BoundingBox]->SetModel(modelStack.GetTopModel());
						Meshes::meshes[(int)MeshType::BoundingBox]->Render(SP);
					break;
				}
				case ColliderType::Sphere:
					SphereCollider* const sphereCollider = static_cast<SphereCollider*>(entity->collider);

					modelStack.PushModel({
						modelStack.Translate(sphereCollider->GetPos()),
						modelStack.Scale(glm::vec3(sphereCollider->GetRadius())),
					});
						Meshes::meshes[(int)MeshType::BoundingSphere]->SetModel(modelStack.GetTopModel());
						Meshes::meshes[(int)MeshType::BoundingSphere]->Render(SP);
					break;
			}
			modelStack.PopModel();
		}
	}

	SP.Set1i("useCustomDiffuseTexIndex", 0);

	///Then render non-opaque entities
	for(std::multimap<int, Entity*>::reverse_iterator iter = entitiesNotOpaque.rbegin(); iter != entitiesNotOpaque.rend(); ++iter){
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
						Meshes::meshes[(int)MeshType::BoundingBox]->SetModel(modelStack.GetTopModel());
						Meshes::meshes[(int)MeshType::BoundingBox]->Render(SP);
					break;
				}
				case ColliderType::Sphere:
					SphereCollider* const sphereCollider = static_cast<SphereCollider*>(entity->collider);

					modelStack.PushModel({
						modelStack.Translate(sphereCollider->GetPos()),
						modelStack.Scale(glm::vec3(sphereCollider->GetRadius())),
					});
						Meshes::meshes[(int)MeshType::BoundingSphere]->SetModel(modelStack.GetTopModel());
						Meshes::meshes[(int)MeshType::BoundingSphere]->Render(SP);
					break;
			}
			modelStack.PopModel();
		}
	}

	SP.Set1i("noNormals", 0);
	SP.Set1i("useCustomColour", 0);
}

EntityFactory* EntityManager::RetrieveEntityFactory(){
	return entityFactory;
}

void EntityManager::SetUpRegionsForStationary(){
	std::vector<Entity*> entitiesToRemove;
	regionManager->SetUpRegionsForStationary(entitiesToRemove);
	for(Entity* const entity: entitiesToRemove){
		DeactivateEntity(entity);
	}
}

void EntityManager::DeactivateEntity(Entity* const& entity){
	if(entity->collider != nullptr){
		colliderManager->DeactivateCollider(entity->collider);
	}

	DeactivateEntityProcedure(entity);

	entityPool->DeactivateObj(entity);
}

void EntityManager::DeactivateEntityProcedure(Entity* const entity){
	static std::unordered_set<Entity*> removedEntities;

	if(std::find(removedEntities.begin(), removedEntities.end(), entity) == removedEntities.end()){
		Node* const node = nodeManager->RetrieveRootNode()->DetachChild(entity);

		if(node == nullptr){
			return (void)printf("Var 'node' is nullptr!");
		}

		node->SetEntity(nullptr);
		regionManager->RetrieveRootRegion()->RemoveNode(node, entity->movable);

		removedEntities.insert(entity);
	}
}

EntityManager::EntityManager():
	isCamDetached(false),
	shldRenderColliders(false),
	elapsedTime(0.0f),
	entityFactory(EntityFactory::GetObjPtr()),
	colliderManager(ColliderManager::GetObjPtr()),
	nodeManager(NodeManager::GetObjPtr()),
	regionManager(RegionManager::GetObjPtr()),
	entityPool(ObjPool<Entity>::GetObjPtr()),
	modelStack()
{
}