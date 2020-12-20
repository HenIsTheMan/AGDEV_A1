#pragma once

#include <Engine.h>

enum struct DetailLvl: int{
	High,
	Medium,
	Low,
	Amt
};

class LOD final{
public:
	LOD();

	Model* GetModel(const float distSquared) const;

	///Setters
	void SetDistSquared(const DetailLvl lvl, const float distSquared);
	void SetModel(const DetailLvl lvl, Model* const model);
	void SetDistSquaredAndModel(const DetailLvl lvl, const float distSquared, Model* const model);
private:
	float squaredDists[(int)DetailLvl::Amt]; //Optimization
	Model* models[(int)DetailLvl::Amt];
};