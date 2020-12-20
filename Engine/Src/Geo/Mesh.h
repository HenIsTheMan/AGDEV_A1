#pragma once
#include "Vertex.h"
#include "../Graphics/ShaderProg.h"

class Mesh{ //Single drawable geo
	friend class Model;
public:
	enum struct MeshType{ //Pt, Line, 2D (circle, ring and 2D polygons with >= 3 vertices), 3D (sphere, torus and 3D polyhedrons with >= 3 flat polygonal faces)
		Quad = 0,
		Cube,
		Sphere,
		Cylinder,
		Amt
	};
	enum struct TexType{
		Diffuse = 0,
		Spec,
		Emission,
		Reflection,
		Bump,
		Amt
	};

	Mesh();
	Mesh(const MeshType& type, const int& primitive, const std::initializer_list<std::tuple<str, TexType, uint>>& iL);

	///Rule of 5 (prevents shallow copy)
	Mesh(const Mesh& mesh);
	Mesh(Mesh&& mesh) noexcept;
	Mesh& operator=(const Mesh& mesh);
	Mesh& operator=(Mesh&& mesh) noexcept;
	virtual ~Mesh();

	static int normalDrawCalls;
	static int instancedDrawCalls;
	static int vertexCount;
	static int indexCount;
	static int polygonCount;

	void AddModelMat(const glm::mat4& modelMat);
	void AddColor(const glm::vec3& color);
	void AddDiffuseTexIndex(const int diffuseTexIndex);
	void AddTexMap(const std::tuple<str, TexType, uint>& texMap);

	void ClearModelMats();
	void ClearTexMaps();

	void RemoveModelMat(const size_t& index);
	void RemoveTexMap(str const& texPath);

	void ReserveModelMats(const size_t& size);
	void ResizeModelMats(const size_t& size);
	void ReserveColors(const size_t& size);
	void ResizeColors(const size_t& size);
	void ReserveDiffuseTexIndices(const size_t& size);
	void ResizeDiffuseTexIndices(const size_t& size);

	void SetModel(const glm::mat4& model);
	void SetModelMat(const glm::mat4& modelMat, const ptrdiff_t& index);

	void InstancedRender(ShaderProg& SP, const bool& autoConfig = true);
	virtual void Render(ShaderProg& SP, const bool& autoConfig = true);
protected:
	MeshType type;
	int primitive;
	std::vector<Vertex>* vertices;
	std::vector<uint>* indices;
	std::vector<std::tuple<str, TexType, uint>> texMaps;

	std::vector<glm::mat4> modelMats;
	std::vector<glm::vec3> colors;
	std::vector<int> diffuseTexIndices;

	uint batchVAO;
	uint batchVBO;
	uint batchEBO;
	uint instancingVAO;
	uint instancingVBO;
	uint instancingEBO;
	uint VAO;
	uint VBO;
	uint EBO;

	glm::mat4 model;
	void CreateQuad(); //2x2
	void CreateCube(); //2x2x2
	void CreateSphere(); //2x2x2
	void CreateCylinder(); //2x2x2
};