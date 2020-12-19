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

Node* NodeManager::ActivateNode(){
	return nodePool->ActivateObj();
}

void NodeManager::DeactivateNode(Node* const node){
	nodePool->DeactivateObj(node);
}

NodeManager::NodeManager():
	nodePool(ObjPool<Node>::GetObjPtr())
{
}