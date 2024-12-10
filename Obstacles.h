#pragma once

#include "components/simple_scene.h"
#include <vector>

namespace m1
{
	class Obstacles : public gfxc::SimpleScene
	{
	public:
		void GenerateObstaclesPos();
		Mesh* GenerateCheckPoint(glm::vec3 color);

		std::vector<glm::vec3> treePositions;
		std::vector<glm::vec3> buildingPositions;
		std::vector<glm::vec3> gatePositions;

		Obstacles() {}
	};
}