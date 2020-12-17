#pragma once

#include "Engine.h"

enum struct MeshType{
	Quad = 0,
	Cube,
	Sphere,
	Cylinder,
	Terrain,
	CoinSpriteAni,
	FireSpriteAni,
	QuadLineLoop,
	Amt
};

class Meshes final{
public:
	static Mesh* meshes[(int)MeshType::Amt];
};