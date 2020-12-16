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
}

void RegionControl::Render(ShaderProg& SP) const{
	//Render lines or leaf nodes??
	//Not visible if not active??
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