#include "Collision.h"

#include "../Collision/Collider/Colliders/BoxCollider.h"
#include "../Collision/Collider/Colliders/SphereCollider.h"

#include <glm/gtx/intersect.hpp>

/**
@brief Check two positions are within a box region
*/
bool InBox(glm::vec3 Hit, glm::vec3 B1, glm::vec3 B2, const int Axis)
{
	if (Axis == 1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return true;
	if (Axis == 2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return true;
	if (Axis == 3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return true;
	return false;
}

/**
@brief Check where a line segment between two positions intersects a plane
*/
bool GetIntersection(const float fDst1, const float fDst2, 
	glm::vec3 P1, glm::vec3 P2, glm::vec3 &Hit)
{
	if(fDst1 == fDst2){
		//std::cout << "Here Fail 2\n";
		return false;
	}

	Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
	return true;
}

bool RayBoxIntersectionTest(glm::vec3 line_start, glm::vec3 line_end,
	glm::vec3 minAABB, glm::vec3 maxAABB,
	glm::vec3 &Hit)
{
	if ((GetIntersection(line_start.x - minAABB.x, line_end.x - minAABB.x, line_start, line_end, Hit) &&
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
		InBox(Hit, minAABB, maxAABB, 3)))
	{
		return true;
	}
	return false;

}

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

		//return DetectRayBoxIntersection(rayStart, rayEnd, minAABB, maxAABB, collisionPt);
		return RayBoxIntersectionTest(rayStart, rayEnd, minAABB, maxAABB, collisionPt);
	}
	return false;
}

bool Collision::DetectRayBoxIntersection(const glm::vec3& rayStart, const glm::vec3& rayEnd, const glm::vec3& minAABB, const glm::vec3& maxAABB, glm::vec3& collisionPt){
	std::cout << "Here\n";
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
	if(dist0 * dist1 >= 0.0f || dist0 == dist1){
		return false;
	}

	collisionPt = rayStart + (rayEnd - rayStart) * (-rayEnd / (rayStart - rayEnd));
	return true;
}

bool Collision::WithinBox(const Axis axis, const glm::vec3& minAABB, const glm::vec3& maxAABB, glm::vec3& collisionPt){
	return (axis == Axis::x && collisionPt.z > minAABB.z && collisionPt.z < maxAABB.z && collisionPt.y > minAABB.y && collisionPt.y < maxAABB.y)
		|| (axis == Axis::y && collisionPt.z > minAABB.z && collisionPt.z < maxAABB.z && collisionPt.x > minAABB.x && collisionPt.x < maxAABB.x)
		|| (axis == Axis::z && collisionPt.x > minAABB.x && collisionPt.x < maxAABB.x && collisionPt.y > minAABB.y && collisionPt.y < maxAABB.y);
}