#pragma once

#include <Core.h>

#include "Axis.hpp"

inline static glm::vec3 RotateVecIn2D(const glm::vec3& vec, const float& angleInRad, const Axis& axis){
	switch(axis){
		case Axis::x:
			return glm::vec3(vec.x, vec.y * cos(angleInRad) + vec.z * -sin(angleInRad), vec.y * sin(angleInRad) + vec.z * cos(angleInRad));
		case Axis::y:
			return glm::vec3(vec.x * cos(angleInRad) + vec.z * sin(angleInRad), vec.y, vec.x * -sin(angleInRad) + vec.z * cos(angleInRad));
		case Axis::z:
			return glm::vec3(vec.x * cos(angleInRad) + vec.y * -sin(angleInRad), vec.x * sin(angleInRad) + vec.y * cos(angleInRad), vec.z);
		default:
			(void)puts("Invalid axis input for vec rotation!");
			return glm::vec3();
	}
}