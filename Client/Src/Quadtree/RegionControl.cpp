#include "RegionControl.h"

extern float terrainXScale;
extern float terrainZScale;

RegionControl::RegionControl():
	root(new Region())
{
	root->active = true;
	root->origin = glm::vec2(0.0f);
	root->size = glm::vec2(terrainXScale, terrainZScale);
	InitRegionPool(80);
}

RegionControl::~RegionControl(){
	if(root){
		root->DestroyRegionPool();
		delete root;
		root = nullptr;
	}
}

void Update();
void Render() const;

void AddEntity(Entity* const& entity);
void RemoveEntity(Entity* const& entity);
void DeactivateAndClear();

//Entity* const& EntityManager::FetchEntity(){
//	for(Entity* const& entity: entityList){
//		if(!entity->active){
//			return entity;
//		}
//	}
//	entityList.emplace_back(new Entity());
//	root->AddEntity(entityList.back());
//	(void)puts("1 entity was added to entityList!\n");
//	return entityList.back();
//}
//
//void EntityManager::CreateEntities(const int& amt){ //Shld only be called once in Scene::Init()
//	entityList = std::vector<Entity*>(amt); //Prealloc mem
//	for(int i = 0; i < amt; ++i){
//		entityList[i] = new Entity();
//		root->AddEntity(entityList[i]);
//	}
//}

//void EntityManager::UpdateEntities(UpdateParams& params){
//	//assert(root->entityList->size() != 0);
//	root->Deactivate();
//	//assert(root->entityList->size() != 0);
//	root->Partition();
//	//assert(root->entityList->size() != 0);
//}