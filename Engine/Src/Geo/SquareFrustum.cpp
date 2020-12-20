#include "SquareFrustum.h"

SquareFrustum::SquareFrustum():
	Mesh(),
	vertexPos{}
{
}

void SquareFrustum::Init(){ //??
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 10000 * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void SquareFrustum::Update(glm::vec3 (&const vertexPos)[24]){
	for(int i = 0; i < 24; ++i){
		this->vertexPos[i] = vertexPos[i];
	}
}

void SquareFrustum::Render(ShaderProg& SP, const bool& autoConfig){ //??
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 24 * sizeof(glm::vec3), vertexPos);

	glDrawArrays(GL_LINES, 0, 24);
}