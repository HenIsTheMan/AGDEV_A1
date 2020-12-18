#include "LOD.h"

LOD::LOD():
	squaredDists(),
	models()
{
}

Model* LOD::GetModel(const float distSquared) const{
	size_t size = sizeof(squaredDists) / sizeof(squaredDists[0]);
	for(size_t i = 0; i < size; ++i){
		if(distSquared <= squaredDists[i]){
			return models[i];
		}
	}
	return nullptr;
}

void LOD::SetDistSquared(const DetailLvl lvl, const float distSquared){
	squaredDists[(int)lvl] = distSquared;
}

void LOD::SetModel(const DetailLvl lvl, Model* const model){
	models[(int)lvl] = model;
}

void LOD::SetDistSquaredAndModel(const DetailLvl lvl, const float distSquared, Model* const model){
	squaredDists[(int)lvl] = distSquared;
	models[(int)lvl] = model;
}