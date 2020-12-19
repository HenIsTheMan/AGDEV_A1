#pragma once

#include <Core.h>

static float EaseInOutCubic(const float x){
	return x < 0.5f ? 4.0f * x * x * x : 1.0f - (float)glm::pow(-2.0f * x + 2.0f, 3.0f) * 0.5f;
}