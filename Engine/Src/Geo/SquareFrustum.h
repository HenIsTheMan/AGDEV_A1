#pragma once

#include "Mesh.h"

class SquareFrustum final: public Mesh{
public:
	SquareFrustum();
	~SquareFrustum() = default;

	void Update(const glm::vec3 (&vertexPos)[24]);
	void Render(ShaderProg& SP, const bool& autoConfig = true) override;
private:
	glm::vec3 vertexPos[24];
};