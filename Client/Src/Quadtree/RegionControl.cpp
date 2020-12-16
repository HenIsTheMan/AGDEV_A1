#include "RegionControl.h"

extern float terrainXScale;
extern float terrainZScale;

RegionControl::~RegionControl(){
	if(rootRegion){
		rootRegion->DestroyRegionPool();
		delete rootRegion;
		rootRegion = nullptr;
	}
}

void RegionControl::Update(){
	rootRegion->ClearMovableAndDeactivateChildren();
	rootRegion->Partition(true);
}

void RegionControl::Render(ShaderProg& SP){
	//Render lines or leaf nodes??
	//Not visible if not active??

	rootRegion->Render(SP);
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
	rootRegion->ReserveStationaryNodes(size);
}

void RegionControl::ReserveMovableNodes(const size_t& size){
	rootRegion->ReserveMovableNodes(size);
}

void RegionControl::InitRegionPool(const size_t& size){
	rootRegion->InitRegionPool(size);
}

void RegionControl::SetUpRegionsForStationary(){
	rootRegion->Partition(false);
}

RegionControl::RegionControl():
	rootRegion(new Region())
{
	rootRegion->active = true;
	rootRegion->origin = glm::vec2(0.0f);
	rootRegion->size = glm::vec2(terrainXScale, terrainZScale);
}