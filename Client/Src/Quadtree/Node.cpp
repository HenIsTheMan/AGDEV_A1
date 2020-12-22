#include "Node.h"

Node::Node():
	visible(true),
	children(),
	parent(nullptr),
	entity(nullptr),
	localTranslation(glm::mat4(1.0f)),
	localRotation(glm::mat4(1.0f)),
	localScaling(glm::mat4(1.0f)),
	worldTransform(glm::mat4(1.0f)),
	worldTransformNoScale(glm::mat4(1.0f))
{
}

Node::~Node(){
	entity = nullptr; //Deleted elsewhere
	parent = nullptr; //Deleted elsewhere
}

void Node::Update(){
	const glm::mat4 localTransformNoScale = localTranslation * localRotation;

	if(parent){
		worldTransformNoScale = parent->worldTransformNoScale * localTransformNoScale;
		worldTransform = parent->worldTransformNoScale * localTransformNoScale * localScaling;
	} else{
		worldTransformNoScale = localTransformNoScale;
		worldTransform = localTransformNoScale * localScaling;
	}

	if(entity){
		entity->SetPos(glm::vec3(worldTransform[3]));
	}

	for(Node* const child: children){
		child->Update();
	}
}

void Node::Reset(){
	//??
}

void Node::AddChild(Node* const child){
	if(child == nullptr){
		return (void)printf("Var 'child' is a nullptr");
	}

	child->parent = this;
	children.emplace_back(child);
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

void Node::LocalTranslate(const glm::vec3& localTranslate){
	localTranslation = glm::translate(localTranslation, localTranslate);
}

void Node::LocalRotate(const glm::vec4& localRotate){
	localRotation = glm::rotate(localRotation, glm::radians(localRotate.w), glm::vec3(localRotate));
}

void Node::LocalScale(const glm::vec3& localScale){
	localScaling = glm::scale(localScaling, localScale);
}

bool Node::GetVisible() const{
	return visible;
}

const Entity* Node::GetEntity() const{
	return entity;
}

void Node::SetVisible(const bool visible){
	this->visible = visible;
}

void Node::SetEntity(Entity* const entity){
	this->entity = entity;
}