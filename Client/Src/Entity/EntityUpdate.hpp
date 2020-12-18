#pragma once

#include "Entity.h"

#include "../Shared/RotateVecIn2D.hpp"

inline static void UpdatePlayerHorizVel(Entity* const player){
	int leftRight = (int)Key(GLFW_KEY_A) - (int)Key(GLFW_KEY_D);
	int frontBack = (int)Key(GLFW_KEY_W) - (int)Key(GLFW_KEY_S);

	if(leftRight == 0 && frontBack == 0){
		player->vel = glm::vec3();
		return;
	}

	player->moveSpd = 400.0f;
	const glm::vec3 xzFacingDir = glm::vec3(player->facingDir.x, 0.f, player->facingDir.z);

	glm::vec3&& change = glm::vec3((float)frontBack, 0.f, (float)frontBack) * xzFacingDir + (float)leftRight * RotateVecIn2D(xzFacingDir, glm::radians(90.0f), Axis::y);
	if(change != glm::vec3(0.f)){
		change = normalize(change);
	}

	player->vel = change * player->moveSpd;
}

inline static void UpdatePlayerVertVel(Entity* const player){
}