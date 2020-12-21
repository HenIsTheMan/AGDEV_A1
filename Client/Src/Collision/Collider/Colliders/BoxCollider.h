#pragma once

#include <Core.h>

#include "../Collider.h"

class BoxCollider final: public Collider{
public:
	BoxCollider();
	BoxCollider(const glm::vec3& pos, const glm::vec3& scale);

	float CalcDiagLen();
	void Reset() override;

	///Getters
	const glm::vec3& GetPos() const;
	const glm::vec3& GetScale() const;

	///Setters
	void SetPos(const glm::vec3& pos);
	void SetScale(const glm::vec3& scale);
private:
	glm::vec3 pos;
	glm::vec3 scale;
};