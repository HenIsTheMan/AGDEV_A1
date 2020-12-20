#include "Collision.h"

#include "../Collider/Colliders/BoxCollider.h"
#include "../Collider/Colliders/SphereCollider.h"

#include <glm/gtx/intersect.hpp>

bool Collision::DetectCollision(const Entity* const actor, const Entity* const actee){
	if(!actor->collider){
		assert(false && "actor->collider is nullptr");
		return false;
	}
	if(!actee->collider){
		assert(false && "actee->collider is nullptr");
		return false;
	}

	///??
	const glm::vec3 rayStart = actor->pos;
	const glm::vec3 rayEnd = actor->prevPos;

	const glm::vec3 spherePos = actee->pos;
	float sphereRadius;

	if(actor->collider->GetType() == ColliderType::Box){
		sphereRadius = static_cast<BoxCollider*>(actor->collider)->CalcDiagLen() * 0.5f; //??
	} else{
		sphereRadius = static_cast<SphereCollider*>(actor->collider)->GetRadius();
	}

	glm::vec3 intersectionPt0;
	glm::vec3 intersectionPt1;
	glm::vec3 intersectionNormal0;
	glm::vec3 intersectionNormal1;

	if(glm::intersectLineSphere(rayStart, rayEnd, spherePos, sphereRadius,
		intersectionPt0, intersectionNormal0, intersectionPt1, intersectionNormal1)
		&& glm::length(intersectionPt0 - rayEnd) <= glm::length(rayStart - rayEnd)
	){
		glm::vec3 minAABB;
		glm::vec3 maxAABB;

		if(actor->collider->GetType() == ColliderType::Box){
			const float colliderScaleFactor = static_cast<BoxCollider*>(actor->collider)->GetScale().x;
			minAABB = actee->pos + colliderScaleFactor * glm::vec3(-1.0f); //??
			maxAABB = actee->pos + colliderScaleFactor * glm::vec3(1.0f); //??
		} else{ //??
			const float colliderRadius = static_cast<SphereCollider*>(actor->collider)->GetRadius();
			minAABB = actee->pos + colliderRadius * glm::vec3(-1.0f); //??
			maxAABB = actee->pos + colliderRadius * glm::vec3(1.0f); //??
		}

		glm::vec3 collisionPt;

		return DetectRayBoxIntersection(rayStart, rayEnd, minAABB, maxAABB, collisionPt);
	}
	return false;
}

bool Collision::DetectRayBoxIntersection(const glm::vec3& rayStart, const glm::vec3& rayEnd, const glm::vec3& minAABB, const glm::vec3& maxAABB, glm::vec3& collisionPt){
	return (GetIntersection(line_start.x - minAABB.x, line_end.x - minAABB.x, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 1))
		|| (GetIntersection(line_start.y - minAABB.y, line_end.y - minAABB.y, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 2))
		|| (GetIntersection(line_start.z - minAABB.z, line_end.z - minAABB.z, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 3))
		|| (GetIntersection(line_start.x - maxAABB.x, line_end.x - maxAABB.x, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 1))
		|| (GetIntersection(line_start.y - maxAABB.y, line_end.y - maxAABB.y, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 2))
		|| (GetIntersection(line_start.z - maxAABB.z, line_end.z - maxAABB.z, line_start, line_end, Hit) &&
		InBox(Hit, minAABB, maxAABB, 3));
}