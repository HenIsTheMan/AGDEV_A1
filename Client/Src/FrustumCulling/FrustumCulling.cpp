#include "FrustumCulling.h"

#include "../Shared/Meshes.h"

FrustumCulling::FrustumCulling():
	view(glm::mat4(1.0f)),
	projection(glm::mat4(1.0f)),
	m_planes(),
	m_points(),
	yMin(0.0f),
	yMax(0.0f)
{
}

void FrustumCulling::Update(const glm::mat4& view, const glm::mat4& projection, const float yMin, const float yMax){
	this->view = view;
	this->projection = projection;
	this->yMin = yMin;
	this->yMax = yMax;

	UpdateFrustum();

	glm::vec3 vertices[24]{
		m_points[0],
		m_points[2],

		m_points[2],
		m_points[3],

		m_points[3],
		m_points[1],

		m_points[1],
		m_points[0],

		m_points[4],
		m_points[6],

		m_points[6],
		m_points[7],

		m_points[7],
		m_points[5],

		m_points[5],
		m_points[4],

		m_points[0],
		m_points[4],

		m_points[2],
		m_points[6],

		m_points[3],
		m_points[7],

		m_points[1],
		m_points[5]
	};
	static_cast<SquareFrustum*>(Meshes::meshes[(int)MeshType::ViewingFrustum])->Update(vertices);
}

bool FrustumCulling::ShldBeVisible(const glm::vec3& minPt, const glm::vec3& maxPt) const{
	for(int i = 0; i < (int)Planes::Count; ++i){
		if(glm::dot(m_planes[i], glm::vec4(minPt.x, minPt.y, minPt.z, 1.0f)) < 0.0 &&
			glm::dot(m_planes[i], glm::vec4(maxPt.x, minPt.y, minPt.z, 1.0f)) < 0.0 &&
			glm::dot(m_planes[i], glm::vec4(minPt.x, maxPt.y, minPt.z, 1.0f)) < 0.0 &&
			glm::dot(m_planes[i], glm::vec4(maxPt.x, maxPt.y, minPt.z, 1.0f)) < 0.0 &&
			glm::dot(m_planes[i], glm::vec4(minPt.x, minPt.y, maxPt.z, 1.0f)) < 0.0 &&
			glm::dot(m_planes[i], glm::vec4(maxPt.x, minPt.y, maxPt.z, 1.0f)) < 0.0 &&
			glm::dot(m_planes[i], glm::vec4(minPt.x, maxPt.y, maxPt.z, 1.0f)) < 0.0 &&
			glm::dot(m_planes[i], glm::vec4(maxPt.x, maxPt.y, maxPt.z, 1.0f)) < 0.0
		){
			return false;
		}
	}

	int out;
	out = 0; for(int i = 0; i < 8; ++i) out += ((m_points[i].x > maxPt.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for(int i = 0; i < 8; ++i) out += ((m_points[i].x < minPt.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for(int i = 0; i < 8; ++i) out += ((m_points[i].y > maxPt.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for(int i = 0; i < 8; ++i) out += ((m_points[i].y < minPt.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for(int i = 0; i < 8; ++i) out += ((m_points[i].z > maxPt.z) ? 1 : 0); if (out == 8) return false;
	out = 0; for(int i = 0; i < 8; ++i) out += ((m_points[i].z < minPt.z) ? 1 : 0); if (out == 8) return false;

	return true;
}

void FrustumCulling::UpdateFrustum(){
	const glm::mat4 m = glm::transpose(projection * view);
	m_planes[(int)Planes::Left] = m[3] + m[0];
	m_planes[(int)Planes::Right] = m[3] - m[0];
	m_planes[(int)Planes::Bottom] = m[3] + m[1];
	m_planes[(int)Planes::Top] = m[3] - m[1];
	m_planes[(int)Planes::Near] = m[3] + m[2];
	m_planes[(int)Planes::Far] = m[3] - m[2];

	glm::vec3 crosses[(int)Planes::Combinations] = {
		glm::cross(glm::vec3(m_planes[(int)Planes::Left]), glm::vec3(m_planes[(int)Planes::Right])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Left]), glm::vec3(m_planes[(int)Planes::Bottom])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Left]), glm::vec3(m_planes[(int)Planes::Top])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Left]), glm::vec3(m_planes[(int)Planes::Near])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Left]), glm::vec3(m_planes[(int)Planes::Far])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Right]), glm::vec3(m_planes[(int)Planes::Bottom])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Right]), glm::vec3(m_planes[(int)Planes::Top])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Right]), glm::vec3(m_planes[(int)Planes::Near])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Right]), glm::vec3(m_planes[(int)Planes::Far])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Bottom]), glm::vec3(m_planes[(int)Planes::Top])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Bottom]), glm::vec3(m_planes[(int)Planes::Near])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Bottom]), glm::vec3(m_planes[(int)Planes::Far])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Top]), glm::vec3(m_planes[(int)Planes::Near])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Top]), glm::vec3(m_planes[(int)Planes::Far])),
		glm::cross(glm::vec3(m_planes[(int)Planes::Near]), glm::vec3(m_planes[(int)Planes::Far]))
	};

	m_points[0] = CalcIntersection<Planes::Left, Planes::Bottom, Planes::Near>(crosses);
	m_points[1] = CalcIntersection<Planes::Left, Planes::Top, Planes::Near>(crosses);
	m_points[2] = CalcIntersection<Planes::Right, Planes::Bottom, Planes::Near>(crosses);
	m_points[3] = CalcIntersection<Planes::Right, Planes::Top, Planes::Near>(crosses);
	m_points[4] = CalcIntersection<Planes::Left, Planes::Bottom, Planes::Far>(crosses);
	m_points[5] = CalcIntersection<Planes::Left, Planes::Top, Planes::Far>(crosses);
	m_points[6] = CalcIntersection<Planes::Right, Planes::Bottom, Planes::Far>(crosses);
	m_points[7] = CalcIntersection<Planes::Right, Planes::Top, Planes::Far>(crosses);
}

template <FrustumCulling::Planes a, FrustumCulling::Planes b, FrustumCulling::Planes c>
inline glm::vec3 FrustumCulling::CalcIntersection(const glm::vec3* crosses) const{
	const glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) * glm::vec3(m_planes[(int)a].w, m_planes[(int)b].w, m_planes[(int)c].w);
	return res * (-1.0f / glm::dot(glm::vec3(m_planes[(int)a]), crosses[ij2k<b, c>::k]));
}