#pragma once

#include <Engine.h>

#include "Node.h"

#include "../ObjPool/ObjPool.h"

class NodeManager final: public Singleton<NodeManager>{
	friend Singleton<NodeManager>;
public:
	~NodeManager();

	void Init(const size_t& inactiveSize, const size_t& activeSize);

	Node* ActivateNode();
	void DeactivateNode(Node* const node);
private:
	ObjPool<Node>* nodePool;

	NodeManager();
};