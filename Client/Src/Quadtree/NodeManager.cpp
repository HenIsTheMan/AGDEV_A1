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

void NodeManager::Update(){
	rootNode->Update();
}

Node* NodeManager::ActivateNode(){
	Node* const node = nodePool->ActivateObj();
	node->Reset();
	return node;
}

void NodeManager::DeactivateNode(Node* const node){
	Node* const myParent = node->parent;
	for(Node* const child: node->children){
		child->parent = myParent;
	}

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