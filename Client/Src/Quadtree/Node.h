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
	Node* DetachChild(const Node* const child);
	Node* DetachChild(const Entity* const entity);
	Node* FindChild(const Entity* const entity);

	Entity* RetrieveEntity();

	///Getters
	bool GetVisible() const;
	const Entity* GetEntity() const;
	const Node* GetParent() const;

	const glm::vec3& GetLocalTranslate() const;
	const glm::vec4& GetLocalRotate() const;
	const glm::vec3& GetLocalScale() const;

	///Setters
	void SetVisible(const bool visible);
	void SetEntity(Entity* const entity);

	void SetLocalTranslate(const glm::vec3& localTranslate);
	void SetLocalRotate(const glm::vec4& localRotate);
	void SetLocalScale(const glm::vec3& localScale);
private:
	bool visible;

	std::vector<Node*> children;
	Node* parent;
	Entity* entity;

	glm::mat4 worldTransform;
	glm::mat4 worldTransformNoScale;

	glm::vec3 localTranslate;
	glm::vec4 localRotate;
	glm::vec3 localScale;
};