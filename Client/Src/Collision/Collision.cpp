#include "Collision.h"

#include "../Collision/Collider/Colliders/BoxCollider.h"
#include "../Collision/Collider/Colliders/SphereCollider.h"

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
	const glm::vec3 rayStart = actor->prevPos;
	const glm::vec3 rayEnd = actor->pos;

	const glm::vec3 spherePos = actee->pos;
	float sphereRadius;

	if(actee->collider->GetType() == ColliderType::Box){
		sphereRadius = static_cast<BoxCollider*>(actee->collider)->CalcDiagLen(); //??
	} else{
		sphereRadius = static_cast<SphereCollider*>(actee->collider)->GetRadius();
	}

	glm::vec3 intersectionPt0;
	glm::vec3 intersectionPt1;
	glm::vec3 intersectionNormal0;
	glm::vec3 intersectionNormal1;

	if(glm::intersectLineSphere(rayStart, rayEnd, spherePos, sphereRadius, intersectionPt0, intersectionNormal0, intersectionPt1, intersectionNormal1)){
		if(!(glm::length(intersectionPt0 - rayEnd) <= glm::length(rayStart - rayEnd))){
			return false;
		}

		glm::vec3 minAABB;
		glm::vec3 maxAABB;

		if(actee->collider->GetType() == ColliderType::Box){
			const glm::vec3& colliderScale = static_cast<BoxCollider*>(actee->collider)->GetScale();
			minAABB = actee->pos - colliderScale;
			maxAABB = actee->pos + colliderScale;
		} else{ //??
			const float colliderRadius = static_cast<SphereCollider*>(actee->collider)->GetRadius();
			minAABB = actee->pos + colliderRadius;
			maxAABB = actee->pos + colliderRadius;
		}

		glm::vec3 collisionPt;

		return DetectRayBoxIntersection(rayStart, rayEnd, minAABB, maxAABB, collisionPt);
	}
	return false;
}

bool Collision::DetectRayBoxIntersection(const glm::vec3& rayStart, const glm::vec3& rayEnd, const glm::vec3& minAABB, const glm::vec3& maxAABB, glm::vec3& collisionPt){
	return (CalcIntersection(rayStart.x - minAABB.x, rayEnd.x - minAABB.x, rayStart, rayEnd, collisionPt)
		&& WithinBox(Axis::x, minAABB, maxAABB, collisionPt))
		|| (CalcIntersection(rayStart.y - minAABB.y, rayEnd.y - minAABB.y, rayStart, rayEnd, collisionPt)
		&& WithinBox(Axis::y, minAABB, maxAABB, collisionPt))
		|| (CalcIntersection(rayStart.z - minAABB.z, rayEnd.z - minAABB.z, rayStart, rayEnd, collisionPt)
		&& WithinBox(Axis::z, minAABB, maxAABB, collisionPt))
		|| (CalcIntersection(rayStart.x - maxAABB.x, rayEnd.x - maxAABB.x, rayStart, rayEnd, collisionPt)
		&& WithinBox(Axis::x, minAABB, maxAABB, collisionPt))
		|| (CalcIntersection(rayStart.y - maxAABB.y, rayEnd.y - maxAABB.y, rayStart, rayEnd, collisionPt)
		&& WithinBox(Axis::y, minAABB, maxAABB, collisionPt))
		|| (CalcIntersection(rayStart.z - maxAABB.z, rayEnd.z - maxAABB.z, rayStart, rayEnd, collisionPt)
		&& WithinBox(Axis::z, minAABB, maxAABB, collisionPt));
}

bool Collision::CalcIntersection(const float dist0, const float dist1, const glm::vec3& rayStart, const glm::vec3& rayEnd, glm::vec3& collisionPt){
	if(/*dist0 * dist1 >= 0.0f || */dist0 == dist1){
		return false;
	}

	collisionPt = rayStart + (rayEnd - rayStart) * (-dist0 / (dist1 - dist0));
	return true;
}

bool Collision::WithinBox(const Axis axis, const glm::vec3& minAABB, const glm::vec3& maxAABB, glm::vec3& collisionPt){
	return (axis == Axis::x && collisionPt.z > minAABB.z && collisionPt.z < maxAABB.z && collisionPt.y > minAABB.y && collisionPt.y < maxAABB.y)
		|| (axis == Axis::y && collisionPt.z > minAABB.z && collisionPt.z < maxAABB.z && collisionPt.x > minAABB.x && collisionPt.x < maxAABB.x)
		|| (axis == Axis::z && collisionPt.x > minAABB.x && collisionPt.x < maxAABB.x && collisionPt.y > minAABB.y && collisionPt.y < maxAABB.y);
}