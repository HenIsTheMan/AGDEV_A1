#include "RegionControl.h"

extern float terrainXScale;
extern float terrainZScale;

RegionControl::~RegionControl(){
	if(root){
		root->DestroyRegionPool();
		delete root;
		root = nullptr;
	}
}

void RegionControl::Update(){
	root->ClearMovableAndDeactivateChildren();
	root->Partition(true);

	//static float burnBT = 0.f;
	//reticleColour = glm::vec4(1.f);
	//cam.canMove = true;

	//for(size_t i = 0; i < entityPool.size(); ++i){
	//	Entity* const& entity = entityPool[i];
	//	if(entity && entity->active){
	//		switch(entity->type){
	//			case Entity::EntityType::Enemy: {
	//				///Enemy movement
	//				entity->pos.y += float(sin(glfwGetTime())) / 5.f;
	//				if(glm::length(cam.GetPos() - entity->pos) >= 50.f){
	//					entity->vel = glm::vec3(glm::rotate(glm::mat4(1.f), glm::radians(PseudorandMinMax(-10.f, 10.f)), {0.f, 1.f, 0.f}) *
	//						glm::vec4(glm::normalize((cam.GetPos() - entity->pos)), 0.f)) * 20.f;
	//				} else{
	//					entity->vel = glm::vec3(0.f);
	//				}

	//				///Ray casting to check for Ray-Sphere intersection
	//				glm::vec3 vec = cam.GetPos() - entity->pos;
	//				const float b = glm::dot(cam.CalcFront(), vec);
	//				const float c = glm::dot(vec, vec) - entity->scale.x * entity->scale.x;
	//				if(b * b - c >= 0.f){
	//					reticleColour = glm::vec4(1.f, 0.f, 0.f, 1.f);
	//				}

	//				///Check for collision with cam
	//				const glm::vec3& relativeVel = cam.trueVel;
	//				const glm::vec3& displacementVec = entity->pos - cam.GetPos();
	//				if(glm::dot(relativeVel, displacementVec) > 0.f && glm::dot(-displacementVec, -displacementVec) <= (entity->scale.x + 5.f) * (entity->scale.x + 5.f)){
	//					cam.canMove = false;
	//					playerHealth -= 2.f;
	//				}
	//				break;
	//			}
	//			case Entity::EntityType::Bullet: {
	//				entity->life -= dt;
	//				if(entity->life < 0.f){
	//					entity->active = false;
	//				}
	//				break;
	//			}
	//			case Entity::EntityType::Coin: {
	//				///Check for collision with cam
	//				const glm::vec3& displacementVec = entity->pos - cam.GetPos();
	//				if(glm::dot(-displacementVec, -displacementVec) <= (entity->scale.x + 5.f) * (entity->scale.x + 5.f)){
	//					soundEngine->play2D("Audio/Sounds/Collect.wav", false);
	//					entity->active = false;
	//					score += 20;
	//				}
	//				break;
	//			}
	//			case Entity::EntityType::Fire: {
	//				const glm::vec3& displacementVec = entity->pos - cam.GetPos();
	//				if(glm::dot(-displacementVec, -displacementVec) <= (entity->scale.x + 5.f) * (entity->scale.x + 5.f)){
	//					if(burnBT <= elapsedTime){
	//						playerHealth -= playerMaxHealth / 10.f;
	//						burnBT = elapsedTime + .5f;
	//					}
	//				}
	//				break;
	//			}
	//			case Entity::EntityType::ShotgunAmmo: {
	//				glm::vec3 N = entity->collisionNormal;
	//				const glm::vec3 displacement = entity->pos - cam.GetPos();
	//				if(glm::dot(displacement, N) < 0.f){
	//					N = -N;
	//				}

	//				const glm::vec3& camPos = cam.GetPos();
	//				const float halfWidth = entity->scale.x;
	//				const float halfHeight = entity->scale.y;
	//				const float halfDepth = entity->scale.z;
	//				glm::vec3 closestPt;
	//				closestPt.x = std::max(entity->pos.x - halfWidth, std::min(camPos.x, entity->pos.x + halfWidth));
	//				closestPt.y = std::max(entity->pos.y - halfHeight, std::min(camPos.y, entity->pos.y + halfHeight));
	//				closestPt.z = std::max(entity->pos.z - halfDepth, std::min(camPos.z, entity->pos.z + halfDepth));
	//				if(glm::dot(cam.trueVel, N) > 0.f && glm::length(closestPt - camPos) < 20.f){
	//					entity->active = false;
	//					if(guns[0]){
	//						guns[0]->SetUnloadedBullets(guns[0]->GetUnloadedBullets() + guns[0]->GetMaxLoadedBullets() / 2);
	//						guns[0]->SetUnloadedBullets(std::min(guns[0]->GetUnloadedBullets(), guns[0]->GetMaxUnloadedBullets()));
	//					}
	//				}
	//				break;
	//			}
	//			case Entity::EntityType::ScarAmmo: {
	//				glm::vec3 N = entity->collisionNormal;
	//				const glm::vec3 displacement = entity->pos - cam.GetPos();
	//				if(glm::dot(displacement, N) < 0.f){
	//					N = -N;
	//				}

	//				const glm::vec3& camPos = cam.GetPos();
	//				const float halfWidth = entity->scale.x;
	//				const float halfHeight = entity->scale.y;
	//				const float halfDepth = entity->scale.z;
	//				glm::vec3 closestPt;
	//				closestPt.x = std::max(entity->pos.x - halfWidth, std::min(camPos.x, entity->pos.x + halfWidth));
	//				closestPt.y = std::max(entity->pos.y - halfHeight, std::min(camPos.y, entity->pos.y + halfHeight));
	//				closestPt.z = std::max(entity->pos.z - halfDepth, std::min(camPos.z, entity->pos.z + halfDepth));
	//				if(glm::dot(cam.trueVel, N) > 0.f && glm::length(closestPt - camPos) < 20.f){
	//					entity->active = false;
	//					if(guns[1]){
	//						guns[1]->SetUnloadedBullets(guns[1]->GetUnloadedBullets() + guns[1]->GetMaxLoadedBullets());
	//						guns[1]->SetUnloadedBullets(std::min(guns[1]->GetUnloadedBullets(), guns[1]->GetMaxUnloadedBullets()));
	//					}
	//				}
	//				break;
	//			}
	//			case Entity::EntityType::SniperAmmo: {
	//				glm::vec3 N = entity->collisionNormal;
	//				const glm::vec3 displacement = entity->pos - cam.GetPos();
	//				if(glm::dot(displacement, N) < 0.f){
	//					N = -N;
	//				}

	//				const glm::vec3& camPos = cam.GetPos();
	//				const float halfWidth = entity->scale.x;
	//				const float halfHeight = entity->scale.y;
	//				const float halfDepth = entity->scale.z;
	//				glm::vec3 closestPt;
	//				closestPt.x = std::max(entity->pos.x - halfWidth, std::min(camPos.x, entity->pos.x + halfWidth));
	//				closestPt.y = std::max(entity->pos.y - halfHeight, std::min(camPos.y, entity->pos.y + halfHeight));
	//				closestPt.z = std::max(entity->pos.z - halfDepth, std::min(camPos.z, entity->pos.z + halfDepth));
	//				if(glm::dot(cam.trueVel, N) > 0.f && glm::length(closestPt - camPos) < 20.f){
	//					entity->active = false;
	//					if(guns[2]){
	//						guns[2]->SetUnloadedBullets(guns[2]->GetUnloadedBullets() + guns[2]->GetMaxLoadedBullets() * 2);
	//						guns[2]->SetUnloadedBullets(std::min(guns[2]->GetUnloadedBullets(), guns[2]->GetMaxUnloadedBullets()));
	//					}
	//				}
	//				break;
	//			}
	//		}

	//		for(size_t j = 0; j < entityPool.size(); ++j){
	//			Entity* const& instance = entityPool[j];
	//			if(instance && instance->active){
	//				if(entity->type == Entity::EntityType::Bullet && instance->type == Entity::EntityType::Enemy){
	//					UpdateCollisionBetweenEntities(entity, instance);
	//				} else if(entity->type == Entity::EntityType::Enemy && instance->type == Entity::EntityType::Bullet){
	//					UpdateCollisionBetweenEntities(instance, entity);
	//				} else{
	//					continue;
	//				}
	//			}
	//		}
	//		entity->pos += entity->vel * dt;
	//	}
	//}
}

void RegionControl::Render(ShaderProg& SP) const{
	//Render lines or leaf nodes??
	//Not visible if not active??

	//SP.Use();
	//const size_t& size = entityPool.size();
	//const glm::vec3& camPos = cam.GetPos();
	//const glm::vec3& camFront = cam.CalcFront();

	/////Sort out render order
	//std::map<int, Entity*> entityListForSorting;
	//for(size_t i = 0; i < size; ++i){
	//	Entity* entity = entityPool[i];
	//	if(entity && entity->active){
	//		glm::vec3 displacementVec = entity->pos - camPos;
	//		if(glm::dot(displacementVec, camFront) > 0.f){
	//			entityListForSorting.insert({
	//				(entity->type == Entity::EntityType::Coin || entity->type == Entity::EntityType::Fire ? -9999 : 0)
	//				+ (int)glm::dot(displacementVec, displacementVec), entity});
	//		}
	//	}
	//}

	//for(std::map<int, Entity*>::reverse_iterator iter = entityListForSorting.rbegin(); iter != entityListForSorting.rend(); ++iter){
	//	Entity* const& entity = iter->second;

	//	SP.Set1i("noNormals", 1);
	//	SP.Set1i("useCustomColour", 1);
	//	SP.Set4fv("customColour", entity->colour);
	//	SP.Set1i("useCustomDiffuseTexIndex", 1);
	//	SP.Set1i("customDiffuseTexIndex", entity->diffuseTexIndex);
	//	switch(entity->type){
	//		case Entity::EntityType::Bullet:
	//		case Entity::EntityType::Enemy:
	//			PushModel({
	//				Translate(entity->pos),
	//				Scale(entity->scale),
	//				});
	//			meshes[(int)MeshType::Sphere]->SetModel(GetTopModel());
	//			meshes[(int)MeshType::Sphere]->Render(SP);
	//			break;
	//		case Entity::EntityType::ShotgunAmmo:
	//		case Entity::EntityType::ScarAmmo:
	//		case Entity::EntityType::SniperAmmo:
	//			PushModel({
	//				Translate(entity->pos),
	//				//Rotate(glm::vec4(0.f, 1.f, 0.f, glm::degrees(atan2(entity->collisionNormal.z, entity->collisionNormal.x)))),
	//				Scale(entity->scale),
	//				});
	//			meshes[(int)MeshType::Cube]->SetModel(GetTopModel());
	//			meshes[(int)MeshType::Cube]->Render(SP);
	//			break;
	//		case Entity::EntityType::Coin:
	//			PushModel({
	//				Translate(entity->pos),
	//				Rotate(glm::vec4(0.f, 1.f, 0.f, glm::degrees(atan2(cam.GetPos().x - entity->pos.x, cam.GetPos().z - entity->pos.z)))),
	//				Scale(entity->scale),
	//				});
	//			SP.Set1i("useCustomColour", 0);
	//			SP.Set1i("useCustomDiffuseTexIndex", 0);
	//			meshes[(int)MeshType::CoinSpriteAni]->SetModel(GetTopModel());
	//			meshes[(int)MeshType::CoinSpriteAni]->Render(SP);
	//			break;
	//		case Entity::EntityType::Fire:
	//			PushModel({
	//				Translate(entity->pos + glm::vec3(0.f, entity->scale.y / 2.f, 0.f)),
	//				Rotate(glm::vec4(0.f, 1.f, 0.f, glm::degrees(atan2(cam.GetPos().x - entity->pos.x, cam.GetPos().z - entity->pos.z)))),
	//				Scale(glm::vec3(entity->scale.x, entity->scale.y * 2.f, entity->scale.z)),
	//				});
	//			SP.Set1i("useCustomColour", 0);
	//			SP.Set1i("useCustomDiffuseTexIndex", 0);
	//			meshes[(int)MeshType::FireSpriteAni]->SetModel(GetTopModel());
	//			meshes[(int)MeshType::FireSpriteAni]->Render(SP);
	//			break;
	//	}
	//	SP.Set1i("useCustomDiffuseTexIndex", 0);
	//	SP.Set1i("useCustomColour", 0);
	//	SP.Set1i("noNormals", 0);
	//	PopModel();
	//}
}

const Region* RegionControl::FindRegion(Entity* const& entity, const bool movable){
	return root->FindRegion(entity, movable);
}

void RegionControl::AddEntity(Entity* const& entity, const bool movable){
	root->AddEntity(entity, movable);
}

void RegionControl::RemoveEntity(Entity* const& entity, const bool movable){
	root->RemoveEntity(entity, movable);
}

void RegionControl::ReserveStationaryEntities(const size_t& size){
	root->ReserveStationaryEntities(size);
}

void RegionControl::ReserveMovableEntities(const size_t& size){
	root->ReserveMovableEntities(size);
}

void RegionControl::InitRegionPool(const size_t& size){
	root->InitRegionPool(size);
}

RegionControl::RegionControl():
	root(new Region())
{
	root->active = true;
	root->origin = glm::vec2(0.0f);
	root->size = glm::vec2(terrainXScale, terrainZScale);
}