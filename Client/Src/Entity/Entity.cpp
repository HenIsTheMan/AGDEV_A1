#include "Entity.h"

Entity::Entity():
	type(EntityType::Amt),
	active(false),
	movable(false),
	life(0.f),
	maxLife(0.f),
	colour(glm::vec4(.7f, .4f, .1f, 1.f)),
	diffuseTexIndex(-1),
	scale(glm::vec3(1.f)),

	pos(glm::vec3(0.f)),
	vel(glm::vec3(0.0f)),
	mass(1.f),
	force(glm::vec3(0.f)),

	collider(nullptr),
	facingDir(glm::vec3(0.0f)),
	moveSpd(0.0f),

	xMin(0.0f),
	xMax(0.0f),
	yMin(0.0f),
	yMax(0.0f),
	zMin(0.0f),
	zMax(0.0f)
{
}