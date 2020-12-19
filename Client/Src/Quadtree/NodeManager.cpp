#include "NodeManager.h"

NodeManager::~NodeManager(){
	if(nodePool != nullptr){
		nodePool->Destroy();
		nodePool = nullptr;
	}
	rootNode = nullptr;
}

void NodeManager::Init(const size_t& inactiveSize, const size_t& activeSize){
	nodePool->Init(inactiveSize, activeSize);
	rootNode = ActivateNode();
}

Node* NodeManager::ActivateNode(){
	return nodePool->ActivateObj();
}

void NodeManager::DeactivateNode(Node* const node){
	nodePool->DeactivateObj(node);
}

Node* NodeManager::RetrieveRootNode(){
	return rootNode;
}

NodeManager::NodeManager():
	rootNode(nullptr),
	nodePool(ObjPool<Node>::GetObjPtr())
{
}