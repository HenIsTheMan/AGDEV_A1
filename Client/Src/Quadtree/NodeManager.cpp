#include "NodeManager.h"

std::vector<Node*> NodeManager::activeNodes;
std::vector<Node*> NodeManager::inactiveNodes;

NodeManager::~NodeManager(){
	for(Node*& node: activeNodes){
		if(node != nullptr){
			delete node;
			node = nullptr;
		}
	}

	for(Node*& node: inactiveNodes){
		if(node != nullptr){
			delete node;
			node = nullptr;
		}
	}
}

void NodeManager::Init(const size_t& inactiveSize, const size_t& activeSize){
	activeNodes.reserve(activeSize);

	inactiveNodes.reserve(inactiveSize);
	for(size_t i = 0; i < inactiveSize; ++i){
		inactiveNodes.emplace_back(new Node());
	}
}

Node* NodeManager::ActivateNode(){
	Node* const node = inactiveNodes.front();
	activeNodes.emplace_back(node);
	inactiveNodes.erase(inactiveNodes.begin());
	return node;
}

void NodeManager::DeactivateNode(const Node* const node){
	std::vector<Node*>::iterator iter = std::find(activeNodes.begin(), activeNodes.end(), node);
	if(iter != activeNodes.end()){
		inactiveNodes.emplace_back(*iter);
		activeNodes.erase(iter);
	}
}