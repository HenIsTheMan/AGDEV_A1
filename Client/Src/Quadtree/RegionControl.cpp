#include "RegionControl.h"

extern float dt;

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
	elapsedTime += dt;

	static float BT = 0.0f;
	if(Key(GLFW_KEY_L) && BT <= elapsedTime){
		shldRenderQSP = !shldRenderQSP;
		BT = elapsedTime + .5f;
	}

	rootRegion->ClearMovableAndDeactivateChildren();
	rootRegion->Partition(true);
}

void RegionControl::Render(ShaderProg& SP, const Cam& cam){
	if(shldRenderQSP){
		RenderQSP(SP, cam);
	}
}

void RegionControl::RenderQSP(ShaderProg& SP, const Cam& cam){
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

void RegionControl::GetEntitiesToUpdate(std::vector<Entity*>& movableEntities, std::vector<Entity*>& stationaryEntities){
	rootRegion->GetEntitiesToUpdate(movableEntities, stationaryEntities);
}

void RegionControl::GetEntitiesToRender(std::map<int, Entity*>& entitiesOpaque, std::map<int, Entity*>& entitiesNotOpaque, const Cam& cam){
	rootRegion->GetEntitiesToRender(entitiesOpaque, entitiesNotOpaque, cam);
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
	shldRenderQSP(false),
	elapsedTime(0.0f),
	modelStack(),
	rootRegion(new Region())
{
	rootRegion->active = true;
	rootRegion->origin = glm::vec2(0.0f);
	rootRegion->size = glm::vec2(terrainXScale, terrainZScale);
}