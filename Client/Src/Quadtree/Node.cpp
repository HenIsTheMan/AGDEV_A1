#include "Node.h"

#include "../Collision/Collider/Colliders/BoxCollider.h"
#include "../Collision/Collider/Colliders/SphereCollider.h"

extern float dt;

Node::Node():
	visible(true),
	entity(nullptr),
	parent(nullptr),
	children(),
	localTranslation(glm::vec3()),
	localRotation(glm::quat()),
	localDilation(glm::vec3(1.0f)),
	worldTranslation(glm::vec3()),
	worldRotation(glm::quat()),
	worldDilation(glm::vec3(1.0f))
{
}

Node::~Node(){
	entity = nullptr; //Deleted elsewhere
	parent = nullptr; //Deleted elsewhere
}

void Node::Update(){
	if(parent){
		worldTranslation = parent->worldTranslation + localTranslation;
		worldRotation = parent->worldRotation * localRotation;
		worldDilation = parent->worldDilation * localDilation;
	} else{
		worldTranslation = localTranslation;
		worldRotation = localRotation;
		worldDilation = localDilation;
	}

	if(entity){
		worldTranslation.x = std::min(entity->xMax, std::max(entity->xMin, worldTranslation.x));
		worldTranslation.y = std::min(entity->yMax, std::max(entity->yMin, worldTranslation.y));
		worldTranslation.z = std::min(entity->zMax, std::max(entity->zMin, worldTranslation.z));

		entity->pos = worldTranslation;
		//entity->SetFacingDir();
		entity->scale = worldDilation;

		if(entity->collider != nullptr){
			if(entity->collider->type == ColliderType::Box){
				BoxCollider* const boxCollider = static_cast<BoxCollider*>(entity->collider);
				boxCollider->SetPos(entity->pos);
				//boxCollider->SetFacingDir();
				boxCollider->SetScale(entity->scale);
			} else{
				SphereCollider* const sphereCollider = static_cast<SphereCollider*>(entity->collider);
				sphereCollider->SetPos(entity->pos);
				sphereCollider->SetRadius(entity->scale[0]);
			}
		}
	}

	for(Node* const child: children){
		child->Update();
	}
}

void Node::Reset(){
	visible = true;
	entity = nullptr;
	parent = nullptr;
	children = std::vector<Node*>();
	localTranslation = glm::vec3();
	localRotation = glm::quat();
	localDilation = glm::vec3(1.0f);
	worldTranslation = glm::vec3();
	worldRotation = glm::quat();
	worldDilation = glm::vec3(1.0f);
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
	localTranslation += localTranslate;
}

void Node::LocalRotate(const glm::quat& localRotate){
	localRotation = localRotate * localRotation;
}

void Node::LocalScale(const glm::vec3& localScale){
	localDilation += localScale;
}

bool Node::GetVisible() const{
	return visible;
}

const Entity* Node::GetEntity() const{
	return entity;
}

const glm::vec3& Node::GetLocalTranslation() const{
	return localTranslation;
}

const glm::quat& Node::GetLocalRotation() const{
	return localRotation;
}

const glm::vec3& Node::GetLocalDilation() const{
	return localDilation;
}

void Node::SetVisible(const bool visible){
	this->visible = visible;
}

void Node::SetEntity(Entity* const entity){
	this->entity = entity;
}

void Node::SetLocalTranslation(const glm::vec3& localTranslation){
	this->localTranslation = localTranslation;
}

void Node::SetLocalRotation(const glm::quat& localRotation){
	this->localRotation = localRotation;
}

void Node::SetLocalDilation(const glm::vec3& localDilation){
	this->localDilation = localDilation;
}