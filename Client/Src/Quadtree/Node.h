#pragma once

#include <Core.h>

#include "../Entity/Entity.h"

class Node final{
	friend class Region;
public:
	Node();
	~Node();

	void Update();
	void Reset();

	void AddChild(Node* const child);
	Node* DetachChild(const Entity* const entity);
	Node* FindChild(const Entity* const entity);
	Entity* RetrieveEntity();

	void LocalTranslate(const glm::vec3& localTranslate);
	void LocalRotate(const glm::vec4& localRotate);
	void LocalScale(const glm::vec3& localScale);

	///Getters
	bool GetVisible() const;
	const Entity* GetEntity() const;

	///Setters
	void SetVisible(const bool visible);
	void SetEntity(Entity* const entity);
private:
	bool visible;
	Entity* entity;

	Node* parent;
	std::vector<Node*> children;

	glm::mat4 localTranslation;
	glm::mat4 localRotation;
	glm::mat4 localScaling;

	glm::mat4 worldTransform;
	glm::mat4 worldTransformNoScale;
};