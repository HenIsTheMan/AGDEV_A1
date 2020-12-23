#pragma once

#include <Core.h>

#include "../Entity/Entity.h"

class Node final{
	friend class Region;
	friend class NodeManager;
public:
	Node();
	~Node();

	void Update();
	void Reset();

	void AddChild(Node* const child);
	Node* DetachChild(const Entity* const entity);
	Node* FindChild(const Entity* const entity);
	Entity* RetrieveEntity();

	size_t CalcAmtOfChildren() const;

	void LocalTranslate(const glm::vec3& localTranslate);
	void LocalRotate(const glm::quat& localRotate);
	void LocalScale(const glm::vec3& localScale);

	///Getters
	bool GetVisible() const;
	const Entity* GetEntity() const;
	const Node* GetParent() const;
	const glm::vec3& GetLocalTranslation() const;
	const glm::quat& GetLocalRotation() const;
	const glm::vec3& GetLocalDilation() const;

	///Setters
	void SetUseLocalTranslationUpdate(const bool useLocalTranslationUpdate);
	void SetUseLocalRotationUpdate(const bool useLocalRotationUpdate);
	void SetUseLocalDilationUpdate(const bool useLocalDilationUpdate);
	void SetVisible(const bool visible);
	void SetEntity(Entity* const entity);
	void SetLocalTranslation(const glm::vec3& localTranslation);
	void SetLocalRotation(const glm::quat& localRotation);
	void SetLocalDilation(const glm::vec3& localDilation);
	void SetLocalTranslationUpdate(const glm::vec3& localTranslationUpdate);
	void SetLocalRotationUpdate(const glm::quat& localRotationUpdate);
	void SetLocalDilationUpdate(const glm::vec3& localDilationUpdate);
private:
	bool useLocalTranslationUpdate;
	bool useLocalRotationUpdate;
	bool useLocalDilationUpdate;
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

	glm::vec3 localTranslationUpdate;
	glm::quat localRotationUpdate;
	glm::vec3 localDilationUpdate;
};