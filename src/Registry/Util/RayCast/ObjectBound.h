#pragma once

struct ObjectBound
{
	ObjectBound() = default;
	ObjectBound(glm::vec3 pBoundMin, glm::vec3 pBoundMax, glm::vec3 pWorldBoundMin, glm::vec3 pWorldBoundMax, glm::vec3 pRotation) :
		boundMin(pBoundMin), boundMax(pBoundMax), worldBoundMin(pWorldBoundMin), worldBoundMax(pWorldBoundMax), rotation(pRotation) {}
	~ObjectBound() = default;

	glm::vec3 GetCenterWorld() const;

	glm::vec3 boundMin{ glm::vec3() };
	glm::vec3 boundMax{ glm::vec3() };
	glm::vec3 worldBoundMin{ glm::vec3() };
	glm::vec3 worldBoundMax{ glm::vec3() };
	glm::vec3 rotation{ glm::vec3() };
};
std::optional<ObjectBound> MakeBoundingBox(RE::NiNode* a_niobj);
