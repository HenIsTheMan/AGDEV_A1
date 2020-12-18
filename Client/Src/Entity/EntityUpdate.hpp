#pragma once

#include "Entity.h"

static void UpdatePlayerHorizVel(Entity* const player){
	float leftRight = float(Key(GLFW_KEY_A) - Key(GLFW_KEY_D));
	float frontBack = float(Key(GLFW_KEY_W) - Key(GLFW_KEY_S));

	//const glm::vec3&& camFront = CalcFront();
	//const glm::vec3&& xzCamFront = glm::vec3(camFront.x, 0.f, camFront.z);
	//glm::vec3&& frontBackDir = glm::normalize(glm::dot(camFront, glm::normalize(xzCamFront)) * glm::normalize(xzCamFront));
	//frontBackDir.y = 1.f;

	//glm::vec3&& change = glm::vec3(frontBack, 0.f, frontBack) * frontBackDir + leftRight * -CalcRight();
	//if(change != glm::vec3(0.f)){
	//	change = normalize(change);
	//}
	//trueVel = change;
}

static void UpdatePlayerVertVel(Entity* const player){
}