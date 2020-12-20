#include "Collision.h"

bool Collision::DetectCollision(const Entity* const actor, const Entity* const actee){
	///??
	const glm::vec3 rayStart = actor->GetPos();
	const glm::vec3 rayEnd = actor->GetPrevPos();
}