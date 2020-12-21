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

	void Update(const glm::mat4& view, const glm::mat4& projection, const float yMin, const float yMax);

	bool ShldBeVisible(const glm::vec3& minPt, const glm::vec3& maxPt) const;

	float yMin;
	float yMax;
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

	glm::mat4 view;
	glm::mat4 projection;

	glm::vec4 m_planes[(int)Planes::Count];
	glm::vec3 m_points[8];
};