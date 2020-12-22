#include "Entity.h"

void Entity::Reset(){
	type = EntityType::Amt;
	movable = false;
	life = 0.f;
	maxLife = 0.f;
	colour = glm::vec4(.7f, .4f, .1f, 1.f);
	diffuseTexIndex = -1;
	scale = glm::vec3(1.f);

	pos = glm::vec3();
	vel = glm::vec3();
	mass = 1.f;
	force = glm::vec3();

	collider = nullptr;
	facingDir = glm::vec3();
	moveSpd = 0.0f;

	xMin = -FLT_MAX;
	xMax = FLT_MAX;
	yMin = -FLT_MAX;
	yMax = FLT_MAX;
	zMin = -FLT_MAX;
	zMax = FLT_MAX;

	prevPos = glm::vec3();
}

const glm::vec3& Entity::GetPos() const{
	return pos;
}

const glm::vec3& Entity::GetScale() const{
	return scale;
}

const glm::vec3& Entity::GetFacingDir() const{
	return facingDir;
}

void Entity::SetFacingDir(const glm::vec3& facingDir){
	this->facingDir = facingDir;
}

void Entity::SetPos(const glm::vec3& pos){
	this->pos = pos;
}

void Entity::SetScale(const glm::vec3& scale){
	this->scale = scale;
}

Entity::Entity():
	type(EntityType::Amt),
	movable(false),
	life(0.f),
	maxLife(0.f),
	colour(glm::vec4(.7f, .4f, .1f, 1.f)),
	diffuseTexIndex(-1),
	scale(glm::vec3(1.f)),

	pos(glm::vec3()),
	vel(glm::vec3()),
	mass(1.f),
	force(glm::vec3()),

	collider(nullptr),
	facingDir(glm::vec3()),
	moveSpd(0.0f),

	xMin(0.0f),
	xMax(0.0f),
	yMin(0.0f),
	yMax(0.0f),
	zMin(0.0f),
	zMax(0.0f),

	prevPos(glm::vec3())
{
}