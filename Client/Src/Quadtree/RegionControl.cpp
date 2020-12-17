#include "RegionControl.h"

extern float terrainXScale;
extern float terrainYScale;
extern float terrainZScale;

RegionControl::~RegionControl(){
	if(rootRegion){
		rootRegion->DestroyRegionPool();
		delete rootRegion;
		rootRegion = nullptr;
	}
}

void RegionControl::Update(){
	static bool state = false;
	if(Key(GLFW_KEY_Y)){
		state = true;
	}
	if(!state){
		return;
	}
	std::cout << "Here\n";

	rootRegion->ClearMovableAndDeactivateChildren();
	rootRegion->Partition(true);
}

void RegionControl::RenderEntities(ShaderProg& SP, const Cam& cam){
	SP.Set1i("noNormals", 1);
	SP.Set1i("useCustomColour", 1);
	SP.Set1i("useCustomDiffuseTexIndex", 1);

	///Use std::map so render order is correct
	std::map<int, Entity*> entitiesOpaque;
	std::map<int, Entity*> entitiesNotOpaque;
	rootRegion->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);

	std::cout << "entitiesNotOpaque size: " << entitiesNotOpaque.size() << '\n'; //No. varies due to optimisation??

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

void RegionControl::RenderQSP(ShaderProg& SP, const Cam& cam){
	std::vector<Region*> leaves;
	rootRegion->GetLeaves(SP, leaves);
	const size_t size = leaves.size();

	std::cout << "leaves size: " << size << '\n';

	if(!size){
		return;
	}

	SP.Set1i("noNormals", 1);
	SP.Set1i("useCustomDiffuseTexIndex", 0);
	SP.Set1i("useCustomColour", 1);

	for(size_t i = 0; i < size; ++i){
		const Region* const leaf = leaves[i];
		SP.Set4fv("customColour", leaf->color);

		modelStack.PushModel({
			modelStack.Translate(glm::vec3(leaf->origin[0], terrainYScale, leaf->origin[1])),
			modelStack.Rotate(glm::vec4(1.0f, 0.0f, 0.0f, 90.0f)),
			modelStack.Scale(glm::vec3(leaf->size[0] * 0.5f, leaf->size[1] * 0.5f, 1.0f))
		});
			Meshes::meshes[(int)MeshType::QuadLineLoop]->SetModel(modelStack.GetTopModel());
			Meshes::meshes[(int)MeshType::QuadLineLoop]->Render(SP);
		modelStack.PopModel();
	}

	SP.Set1i("noNormals", 0);
}

const Region* RegionControl::FindRegion(Node* const& node, const bool movable){
	return rootRegion->FindRegion(node, movable);
}

void RegionControl::AddNode(Node* const& node, const bool movable){
	rootRegion->AddNode(node, movable);
}

void RegionControl::RemoveNode(Node* const& node, const bool movable){
	rootRegion->RemoveNode(node, movable);
}

void RegionControl::ReserveStationaryNodes(const size_t& size){
	for(Region* const region : Region::regionPool){
		region->stationaryNodes.reserve(size);
	}
}

void RegionControl::ReserveMovableNodes(const size_t& size){
	for(Region* const region: Region::regionPool){
		region->movableNodes.reserve(size);
	}
}

void RegionControl::InitRegionPool(const size_t& size){
	rootRegion->InitRegionPool(size);
}

void RegionControl::SetUpRegionsForStationary(){
	rootRegion->Partition(false);
}

RegionControl::RegionControl():
	modelStack(),
	rootRegion(new Region())
{
	rootRegion->active = true;
	rootRegion->origin = glm::vec2(0.0f);
	rootRegion->size = glm::vec2(terrainXScale, terrainZScale);
}