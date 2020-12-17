#include "LOD.h"

LOD::LOD():
	dists(),
	models()
{
}

Model* LOD::GetModel(const float dist) const{
	size_t size = sizeof(dists) / sizeof(dists[0]);
	for(size_t i = 0; i < size; ++i){
		if(dist <= dists[i]){
			return models[i];
		}
	}
	return nullptr;
}

void LOD::SetDist(const DetailLvl lvl, const float dist){
	dists[(int)lvl] = dist;
}

void LOD::SetModel(const DetailLvl lvl, Model* const model){
	models[(int)lvl] = model;
}

void LOD::SetDistAndModel(const DetailLvl lvl, const float dist, Model* const model){
	dists[(int)lvl] = dist;
	models[(int)lvl] = model;
}