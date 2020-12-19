#pragma once

#include <Engine.h>

#include "Node.h"

class NodeManager final: public Singleton<NodeManager>{
	friend Singleton<NodeManager>;
public:
	~NodeManager();

	void Init(const size_t& inactiveSize, const size_t& activeSize);

	Node* ActivateNode();
	void DeactivateNode(const Node* const node);
private:
	static std::vector<Node*> activeNodes;
	static std::vector<Node*> inactiveNodes;

	NodeManager() = default;
};