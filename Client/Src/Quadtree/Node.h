#pragma once

#include <Core.h>

#include "../Entity/Entity.h"

class Node final{
public:
	Node();
	~Node();

	void AddChild(const Node* const child);
	Node* DetachChild(const Node* const child);
	void RemoveChild(const Node* const child);
	void RemoveAllChildren();

	///Getters
	bool GetUseLocalTransformUpdates() const;
	bool GetVisible() const;
	const Node* GetChild(const Node* const child) const;
	const Node* GetParent() const;
	const Entity* GetEntity() const;

	///Setters
	void SetUseLocalTransformUpdates(const bool useLocalTransformUpdates);
	void SetVisible(const bool visible);
	void SetParent(const Node* const parent);
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

	glm::vec3 localTranslateUpdate;
	glm::vec4 localRotateUpdate;
	glm::vec3 localScaleUpdate;
};