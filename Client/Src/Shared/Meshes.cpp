#include "Meshes.h"

Mesh* Meshes::meshes[(int)MeshType::Amt] = {
	new Mesh(Mesh::MeshType::Quad, GL_TRIANGLES, {
		{"Imgs/BG.jpg", Mesh::TexType::Diffuse, 0},
		{"Imgs/Scope.png", Mesh::TexType::Diffuse, 0},
		{"Imgs/Heart.png", Mesh::TexType::Diffuse, 0},
		{"Imgs/Slot.png", Mesh::TexType::Diffuse, 0},
		{"Imgs/ReticleMain.png", Mesh::TexType::Diffuse, 0},
		{"Imgs/ReticleShotgun.png", Mesh::TexType::Diffuse, 0},
		{"Imgs/ReticleScar.png", Mesh::TexType::Diffuse, 0},
	}),
	new Mesh(Mesh::MeshType::Cube, GL_TRIANGLES, {
	}),
	new Mesh(Mesh::MeshType::Sphere, GL_TRIANGLE_STRIP, {
		{"Imgs/Enemy.jpg", Mesh::TexType::Diffuse, 0},
	}),
	new Mesh(Mesh::MeshType::Cylinder, GL_TRIANGLE_STRIP, {
	}),
	new Terrain("Imgs/hMap.raw", 8.f, 8.f),
	new SpriteAni(1, 6),
	new SpriteAni(4, 8),
	new Mesh(Mesh::MeshType::Quad, GL_LINES, {
	}),
};