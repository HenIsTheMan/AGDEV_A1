#pragma once

#include "Mesh.h"

class SquareFrustum final: public Mesh{
public:
	SquareFrustum();
	~SquareFrustum() = default;

	void Init();
	void Update(glm::vec3 (&const vertexPos)[24]);
	void Render(ShaderProg& SP, const bool& autoConfig = true) override;
private:
	glm::vec3 vertexPos[24];
};