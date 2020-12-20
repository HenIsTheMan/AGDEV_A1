#include "SquareFrustum.h"

SquareFrustum::SquareFrustum():
	Mesh(),
	vertexPos{}
{
}

void SquareFrustum::Update(const glm::vec3 (&vertexPos)[24]){
	for(int i = 0; i < 24; ++i){
		this->vertexPos[i] = vertexPos[i];
	}
}

void SquareFrustum::Render(ShaderProg& SP, const bool& autoConfig){
	if(!VAO){ //Lazy Initialization (Init on 1st use)
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 24 * sizeof(glm::vec3), vertexPos);

	glDrawArrays(GL_LINES, 0, 24);
	++normalDrawCalls;
	vertexCount += 24;
	polygonCount += 8;
}