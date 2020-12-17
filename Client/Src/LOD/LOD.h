#pragma once

#include <Engine.h>

enum struct DetailLvl: int{
	Low,
	Medium,
	High,
	Amt
};

class LOD final{
public:
	LOD();

	const Model* GetModel(const float dist) const;

	///Setters
	void SetDist(const DetailLvl lvl, const float dist);
	void SetModel(const DetailLvl lvl, const Model* const model);
private:
	float dists[(int)DetailLvl::Amt];
	const Model* models[(int)DetailLvl::Amt];
};