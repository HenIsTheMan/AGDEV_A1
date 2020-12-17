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

	Model* GetModel(const float dist) const;

	///Setters
	void SetDist(const DetailLvl lvl, const float dist);
	void SetModel(const DetailLvl lvl, Model* const model);
	void SetDistAndModel(const DetailLvl lvl, const float dist, Model* const model);
private:
	float dists[(int)DetailLvl::Amt];
	Model* models[(int)DetailLvl::Amt];
};