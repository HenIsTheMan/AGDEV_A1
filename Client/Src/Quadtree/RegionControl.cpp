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

const Region* RegionControl::FindRegion(Node* const& node, const bool movable){
	return root->FindRegion(node, movable);
}

void RegionControl::AddNode(Node* const& node, const bool movable){
	root->AddNode(node, movable);
}

void RegionControl::RemoveNode(Node* const& node, const bool movable){
	root->RemoveNode(node, movable);
}

void RegionControl::ReserveStationaryNodes(const size_t& size){
	root->ReserveStationaryNodes(size);
}

void RegionControl::ReserveMovableNodes(const size_t& size){
	root->ReserveMovableNodes(size);
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