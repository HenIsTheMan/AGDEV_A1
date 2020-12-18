#pragma once

#include "Entity.h"

static void UpdatePlayerHorizVel(Entity* const player){
	//int leftRight = (int)Key(GLFW_KEY_A) - (int)Key(GLFW_KEY_D);
	//int frontBack = (int)Key(GLFW_KEY_W) - (int)Key(GLFW_KEY_S);

	//if(leftRight == 0 && frontBack == 0){
	//	player->spd = 0.0f;
	//	return;
	//}

	//player->spd = 400.0f;
	//player->dir = glm::normalize(glm::vec3((float)leftRight, 0.0f, (float)frontBack));
}

static void UpdatePlayerVertVel(Entity* const player){
}