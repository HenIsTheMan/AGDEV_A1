#pragma once

#include "Mesh.h"

class SquareFrustum final: public Mesh{
public:
	SquareFrustum();
	~SquareFrustum() = default;

	void Update();
	void Render(ShaderProg& SP, const bool& autoConfig = true) override;
};