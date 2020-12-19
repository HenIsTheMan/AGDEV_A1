#include "RegionManager.h"

extern float dt;

extern float terrainXScale;
extern float terrainYScale;
extern float terrainZScale;

RegionManager::~RegionManager(){
	if(rootRegion){
		rootRegion->DestroyRegionPool();
		delete rootRegion;
		rootRegion = nullptr;
	}
}

void RegionManager::Update(){
	elapsedTime += dt;

	static float BT = 0.0f;
	if(Key(GLFW_KEY_L) && BT <= elapsedTime){
		shldRenderQuadtree = !shldRenderQuadtree;
		BT = elapsedTime + .5f;
	}

	rootRegion->ClearMovableAndDeactivateChildren();
	rootRegion->Partition(true);
}

void RegionManager::Render(ShaderProg& SP, const Cam& cam){
	if(shldRenderQuadtree){
		RenderQuadtree(SP, cam);
	}
}

void RegionManager::RenderQuadtree(ShaderProg& SP, const Cam& cam){
	std::vector<Region*> leaves;
	rootRegion->GetLeaves(SP, leaves);
	const size_t size = leaves.size();

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
			modelStack.Translate(glm::vec3(leaf->origin[0], terrainYScale * 4.0f, leaf->origin[1])),
			modelStack.Rotate(glm::vec4(1.0f, 0.0f, 0.0f, 90.0f)),
			modelStack.Scale(glm::vec3(leaf->size[0] * 0.5f, leaf->size[1] * 0.5f, 1.0f))
		});
			Meshes::meshes[(int)MeshType::QuadRegion]->SetModel(modelStack.GetTopModel());
			Meshes::meshes[(int)MeshType::QuadRegion]->Render(SP);
		modelStack.PopModel();
	}

	SP.Set1i("noNormals", 0);
}

void RegionManager::GetEntitiesToUpdate(std::vector<Entity*>& movableEntities, std::vector<Entity*>& stationaryEntities){
	rootRegion->GetEntitiesToUpdate(movableEntities, stationaryEntities);
}

void RegionManager::GetEntitiesToRender(std::map<int, Entity*>& entitiesOpaque, std::map<int, Entity*>& entitiesNotOpaque, const Cam& cam){
	rootRegion->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
}

const Region* RegionManager::FindRegion(Node* const& node, const bool movable){
	return rootRegion->FindRegion(node, movable);
}

void RegionManager::AddNode(Node* const& node, const bool movable){
	rootRegion->AddNode(node, movable);
}

void RegionManager::RemoveNode(Node* const& node, const bool movable){
	rootRegion->RemoveNode(node, movable);
}

void RegionManager::InitRegionPool(const size_t& size){
	rootRegion->InitRegionPool(size);
}

RegionManager::RegionManager():
	shldRenderQuadtree(false),
	elapsedTime(0.0f),
	modelStack(),
	rootRegion(new Region())
{
	rootRegion->active = true;
	rootRegion->origin = glm::vec2(0.0f);
	rootRegion->size = glm::vec2(terrainXScale, terrainZScale);
}