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
	void LocalRotate(const glm::quat& localRotate);
	void LocalScale(const glm::vec3& localScale);

	///Getters
	bool GetVisible() const;
	const Entity* GetEntity() const;
	const glm::vec3& GetLocalTranslation() const;
	const glm::quat& GetLocalRotation() const;
	const glm::vec3& GetLocalDilation() const;

	///Setters
	void SetVisible(const bool visible);
	void SetEntity(Entity* const entity);
	void SetLocalTranslation(const glm::vec3& localTranslation);
	void SetLocalRotation(const glm::quat& localRotation);
	void SetLocalDilation(const glm::vec3& localDilation);
private:
	bool visible;
	Entity* entity;

	Node* parent;
	std::vector<Node*> children;

	glm::vec3 localTranslation;
	glm::quat localRotation;
	glm::vec3 localDilation;

	glm::vec3 worldTranslation;
	glm::quat worldRotation;
	glm::vec3 worldDilation;
};