#include "LOD.h"

LOD::LOD():
	dists(),
	models()
{
}

const Model* LOD::GetModel(const float dist) const{
	size_t size = sizeof(dists) / sizeof(dists[0]);
	for(size_t i = 0; i < size; ++i){
		if(dist < dists[i]){
			return models[i];
		}
	}

	//??
}

void LOD::SetDist(const DetailLvl lvl, const float dist){
	dists[(int)lvl] = dist;
}

void LOD::SetModel(const DetailLvl lvl, const Model* const model){
	models[(int)lvl] = model;
}