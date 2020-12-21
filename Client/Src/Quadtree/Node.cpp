#include "Node.h"

Node::Node():
	visible(false),
	children(),
	parent(nullptr),
	entity(nullptr),
	worldTransform(glm::mat4(1.0f)),
	worldTransformNoScale(glm::mat4(1.0f)),
	localTranslate(glm::vec3(0.0f)),
	localRotate(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)),
	localScale(glm::vec3(1.0f))
{
}

Node::~Node(){
	entity = nullptr; //Deleted elsewhere
	parent = nullptr; //Deleted elsewhere
}

void Node::Update(){
	const glm::mat4 localTransformNoScale = glm::translate(glm::mat4(1.0f), localTranslate) * glm::rotate(glm::mat4(1.0f), localRotate.w, glm::vec3(localRotate)); //??

	if(parent){
		worldTransformNoScale = parent->worldTransformNoScale * localTransformNoScale;
		worldTransform = parent->worldTransformNoScale * localTransformNoScale * glm::scale(glm::mat4(1.0f), localScale);
	} else{
		worldTransformNoScale = localTransformNoScale;
		worldTransform = localTransformNoScale * glm::scale(glm::mat4(1.0f), localScale);
	}

	if(entity){
		entity->SetPos(glm::vec3(worldTransform[3]));
	}

	for(Node* const child: children){
		child->Update();
	}
}

void Node::AddChild(Node* const child){
	if(child == nullptr){
		return (void)printf("Var 'child' is a nullptr");
	}

	child->parent = this;
	children.emplace_back(child);
}

Node* Node::DetachChild(const Node* const child){
	if(!children.size()){ //Optimization
		return nullptr;
	}

	for(std::vector<Node*>::iterator iter = children.begin(); iter != children.end(); ++iter){
		Node* const node = *iter;
		if(node == child){
			node->parent = nullptr;
			children.erase(iter);
			return node;
		}

		Node* const childNode = node->DetachChild(child);
		if(childNode != nullptr){
			return childNode;
		}
	}
	return nullptr;
}

Node* Node::DetachChild(const Entity* const entity){
	if(!children.size()){ //Optimization
		return nullptr;
	}

	for(std::vector<Node*>::iterator iter = children.begin(); iter != children.end(); ++iter){
		Node* const node = *iter;
		if(node->GetEntity() == entity){
			node->parent = nullptr;
			children.erase(iter);
			return node;
		}

		Node* const childNode = node->DetachChild(entity);
		if(childNode != nullptr){
			return childNode;
		}
	}
	return nullptr;
}

Node* Node::FindChild(const Entity* const entity){
	if(!children.size()){ //Optimization
		return nullptr;
	}

	for(Node* const child: children){
		if(child->GetEntity() == entity){
			return child;
		}

		Node* const childNode = child->FindChild(entity);
		if(childNode != nullptr){
			return childNode;
		}
	}
	return nullptr;
}

Entity* Node::RetrieveEntity(){
	return entity;
}

bool Node::GetVisible() const{
	return visible;
}

const Entity* Node::GetEntity() const{
	return entity;
}

const Node* Node::GetParent() const{
	return parent;
}

const glm::vec3& Node::GetLocalTranslate() const{
	return localTranslate;
}

const glm::vec4& Node::GetLocalRotate() const{
	return localRotate;
}

const glm::vec3& Node::GetLocalScale() const{
	return localScale;
}

void Node::SetVisible(const bool visible){
	this->visible = visible;
}

void Node::SetEntity(Entity* const entity){
	this->entity = entity;
}

void Node::SetLocalTranslate(const glm::vec3& localTranslate){
	this->localTranslate = localTranslate;
}

void Node::SetLocalRotate(const glm::vec4& localRotate){
	this->localRotate = localRotate;
}

void Node::SetLocalScale(const glm::vec3& localScale){
	this->localScale = localScale;
}