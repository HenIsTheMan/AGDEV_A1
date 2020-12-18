#pragma once

#include "Entity.h"

#include "../Shared/RotateVecIn2D.hpp"

inline static void UpdatePlayerHoriz(Entity* const player){
	int leftRight = (int)Key(GLFW_KEY_A) - (int)Key(GLFW_KEY_D);
	int frontBack = (int)Key(GLFW_KEY_W) - (int)Key(GLFW_KEY_S);

	if(leftRight == 0 && frontBack == 0){
		player->vel.x = 0.0f;
		player->vel.z = 0.0f;
		return;
	}

	player->moveSpd = 400.0f;
	const glm::vec3 xzFacingDir = glm::vec3(player->facingDir.x, 0.f, player->facingDir.z);

	glm::vec3&& change = glm::vec3((float)frontBack, 0.f, (float)frontBack) * xzFacingDir + (float)leftRight * RotateVecIn2D(xzFacingDir, glm::radians(90.0f), Axis::y);
	if(change != glm::vec3(0.f)){
		change = normalize(change);
	}

	player->vel.x = change.x * player->moveSpd;
	player->vel.z = change.z * player->moveSpd;
}

inline static void UpdatePlayerVert(Entity* const player){
	static bool isJumping = false;

	if(!isJumping && Key(VK_SPACE)){
		player->vel.y = 300.0f;
		isJumping = true;
	}

	player->force.y = -1500.f;
}