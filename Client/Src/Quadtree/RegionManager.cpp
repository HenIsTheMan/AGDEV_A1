#include "RegionManager.h"

extern float dt;

extern float terrainXScale;
extern float terrainYScale;
extern float terrainZScale;

RegionManager::~RegionManager(){
	if(frustumCulling != nullptr){
		delete frustumCulling;
		frustumCulling = nullptr;
	}

	if(regionPool != nullptr){
		regionPool->Destroy();
		regionPool = nullptr;
	}
	rootRegion = nullptr;
}

void RegionManager::Init(const size_t& inactiveSize, const size_t& activeSize){
	regionPool->Init(inactiveSize, activeSize);

	rootRegion = ActivateRegion();
	rootRegion->origin = glm::vec2(0.0f);
	rootRegion->size = glm::vec2(terrainXScale, terrainZScale);
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
	rootRegion->VisibilityCheck(frustumCulling);
}

void RegionManager::Render(ShaderProg& SP){
	if(shldRenderQuadtree){
		RenderQuadtree(SP);
	}
}

void RegionManager::UpdateFrustumCulling(const glm::mat4& view, const glm::mat4& projection, const float yOffset){
	frustumCulling->Update(view, projection, yOffset);
}

Region* RegionManager::ActivateRegion(){
	return regionPool->ActivateObj();
}

void RegionManager::DeactivateRegion(Region* const region){
	regionPool->DeactivateObj(region);
}

Region* RegionManager::RetrieveRootRegion(){
	return rootRegion;
}

void RegionManager::SetUpRegionsForStationary(){
	rootRegion->Partition(false);
}

RegionManager::RegionManager():
	shldRenderQuadtree(false),
	elapsedTime(0.0f),
	modelStack(),
	rootRegion(nullptr),
	regionPool(ObjPool<Region>::GetObjPtr()),
	frustumCulling(new FrustumCulling())
{
}

void RegionManager::RenderQuadtree(ShaderProg& SP){
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

		if(leaf->visible){
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
	}

	SP.Set1i("noNormals", 0);
}