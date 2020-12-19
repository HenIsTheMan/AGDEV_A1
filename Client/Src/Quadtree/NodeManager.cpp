#include "NodeManager.h"

NodeManager::~NodeManager(){
	if(nodePool != nullptr){
		nodePool->Destroy();
		nodePool = nullptr;
	}
}

void NodeManager::Init(const size_t& inactiveSize, const size_t& activeSize){
	nodePool->Init(inactiveSize, activeSize);
}

NodeManager::NodeManager():
	nodePool(ObjPool<Node>::GetObjPtr())
{
}