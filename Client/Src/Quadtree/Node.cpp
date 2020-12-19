#include "Node.h"

Node::Node():
	useLocalTransformUpdates(false),
	visible(false),
	children(),
	parent(nullptr),
	entity(nullptr),
	worldTransform(glm::mat4()),
	worldTransformNoScale(glm::mat4()),
	localTranslate(glm::vec3(0.0f)),
	localRotate(glm::vec4(0.0f)),
	localScale(glm::vec3(0.0f)),
	localTranslationUpdate(glm::vec3(0.0f)),
	localRotationUpdate(glm::vec4(0.0f)),
	localScalingUpdate(glm::vec3(0.0f))
{
}

Node::~Node(){
	entity = nullptr;
	parent = nullptr;
}

void Node::AddChild(Node* const child){
	if(child == nullptr){
		return (void)printf("Var 'child' is a nullptr");
	}

	child->parent = this;
	children.emplace_back(child);
}

Node* Node::DetachChild(const Node* const child){
	for(std::vector<Node*>::iterator iter = children.begin(); iter != children.end(); ++iter){
		Node* const node = *iter;
		if(node == child){
			node->parent = nullptr;
			children.erase(iter);
			return node;
		} else{
			Node* childNode = node->DetachChild(child);
			if(childNode != nullptr){
				return childNode;
			}
		}
	}
	return nullptr;
}

bool Node::DestroyChild(const Node* const child){
	if(!children.size()){
		return false;
	}

	for(std::vector<Node*>::iterator iter = children.begin(); iter != children.end(); ++iter){
		Node* node = *iter;
		if(node && node == child){
			delete node;
			node = nullptr;
			children.erase(iter);
			return true;
		} else if(node->DestroyChild(child)){
			return true;
		}
	}

	return false;
}

void Node::DestroyAllChildren(){
	for(std::vector<Node*>::iterator iter = children.begin(); iter != children.end(); ++iter){
		Node* node = *iter;
		if(node){
			node->DestroyAllChildren();
			delete node;
			node = nullptr;
		}
	}
}

Entity* Node::RetrieveEntity(){
	return entity;
}

bool Node::GetUseLocalTransformUpdates() const{
	return useLocalTransformUpdates;
}

bool Node::GetVisible() const{
	return visible;
}

const Node* Node::GetParent() const{
	return parent;
}

const Entity* Node::GetEntity() const{
	return entity;
}

void Node::SetUseLocalTransformUpdates(const bool useLocalTransformUpdates){
	this->useLocalTransformUpdates = useLocalTransformUpdates;
}

void Node::SetVisible(const bool visible){
	this->visible = visible;
}

void Node::SetParent(Node* const parent){
	this->parent = parent;
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

void Node::SetLocalTranslationUpdate(const glm::vec3& localTranslationUpdate){
	this->localTranslationUpdate = localTranslationUpdate;
}

void Node::SetLocalRotationUpdate(const glm::vec4& localRotationUpdate){
	this->localRotationUpdate = localRotationUpdate;
}

void Node::SetLocalScalingUpdate(const glm::vec3& localScalingUpdate){
	this->localScalingUpdate = localScalingUpdate;
}