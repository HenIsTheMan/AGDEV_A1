#pragma once

#include <Engine.h>

class FrustumCulling final{
public:
	enum struct Planes: int{
		Left,
		Right,
		Bottom,
		Top,
		Near,
		Far,
		Count,
		Combinations = Count * (Count - 1) / 2
	};

	FrustumCulling();

	void Init(const glm::vec3& topRight, const glm::vec3& bottomLeft);
	void Update();
	void Render(ShaderProg& SP);

	bool IsBoxInside(const glm::vec3& minPt, const glm::vec3& maxPt) const;

	///Setters
	void SetModel(const glm::mat4& model);
	void SetView(const glm::mat4& view);
	void SetProjection(const glm::mat4& projection);
private:
	template<Planes i, Planes j>
	struct ij2k final{
		enum{
			k = (int)i * (9 - (int)i) / 2 + (int)j - 1
		};
	};

	void UpdateFrustum();

	template <Planes a, Planes b, Planes c>
	glm::vec3 CalcIntersection(const glm::vec3* crosses) const;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	glm::vec4 m_planes[(int)Planes::Count];
	glm::vec3 m_points[8];
};