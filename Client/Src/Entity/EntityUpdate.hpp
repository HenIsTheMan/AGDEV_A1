#pragma once

#include "Entity.h"

static void UpdatePlayerHorizVel(Entity* const player){
	int leftRight = (int)Key(GLFW_KEY_A) - (int)Key(GLFW_KEY_D);
	int frontBack = (int)Key(GLFW_KEY_W) - (int)Key(GLFW_KEY_S);

	if(leftRight == 0 && frontBack == 0){
		player->spd = 0.0f;
		return;
	}

	player->spd = 400.0f;
	player->dir = glm::normalize(glm::vec3((float)leftRight, 0.0f, (float)frontBack));

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