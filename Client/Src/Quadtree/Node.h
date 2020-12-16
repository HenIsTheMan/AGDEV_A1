#pragma once

#include <Core.h>

#include "../Entity/Entity.h"

class Node final{
public:
	Node();
	~Node();

	void AddChild(Node* const child);
	Node* DetachChild(const Node* const child);
	bool DestroyChild(const Node* const child);
	void DestroyAllChildren();

	///Getters
	bool GetUseLocalTransformUpdates() const;
	bool GetVisible() const;
	const Node* GetParent() const;
	const Entity* GetEntity() const;

	///Setters
	void SetUseLocalTransformUpdates(const bool useLocalTransformUpdates);
	void SetVisible(const bool visible);
	void SetParent(Node* const parent);
	void SetEntity(Entity* const entity);

	void SetLocalTranslate(const glm::vec3& localTranslate);
	void SetLocalRotate(const glm::vec4& localRotate);
	void SetLocalScale(const glm::vec3& localScale);

	void SetLocalTranslationUpdate(const glm::vec3& localTranslationUpdate);
	void SetLocalRotationUpdate(const glm::vec4& localRotationUpdate);
	void SetLocalScalingUpdate(const glm::vec3& localScalingUpdate);
private:
	bool useLocalTransformUpdates;
	bool visible;

	std::vector<Node*> children;
	Node* parent;
	Entity* entity;

	glm::mat4 worldTransform;
	glm::mat4 worldTransformNoScale;

	glm::vec3 localTranslate;
	glm::vec4 localRotate;
	glm::vec3 localScale;

	glm::vec3 localTranslationUpdate;
	glm::vec4 localRotationUpdate;
	glm::vec3 localScalingUpdate;
};