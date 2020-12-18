#pragma once

#include "Entity.h"

#include "../Shared/RotateVecIn2D.hpp"

extern float terrainXScale;
extern float terrainYScale;
extern float terrainZScale;

inline static void UpdatePlayerHoriz(Entity* const player){
	int leftRight = (int)Key(VK_LEFT) - (int)Key(VK_RIGHT);
	int frontBack = (int)Key(VK_UP) - (int)Key(VK_DOWN);

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

	player->xMin = -terrainXScale * 0.5f + 2.f;
	player->xMax = terrainXScale * 0.5f - 2.f;
	player->zMin = -terrainZScale * 0.5f + 2.f;
	player->zMax = terrainZScale * 0.5f - 2.f;
}

inline static void UpdatePlayerVert(Entity* const player){
	static bool isJumping = false;

	//update isJumping??

	if(!isJumping && Key(VK_SPACE)){
		player->vel.y = 300.0f;
		isJumping = true;
	}

	player->yMin = terrainYScale *
		static_cast<Terrain*>(Meshes::meshes[(int)MeshType::Terrain])->GetHeightAtPt(player->pos.x / terrainXScale, player->pos.y / terrainZScale, false) + player->scale.y * 0.5f;
}