#pragma once

#include "components/simple_scene.h"
#include <vector>

namespace m1
{
	class Drone : public gfxc::SimpleScene
	{
	public:

		void Drone::createDrone();

		std::vector<VertexFormat> verticesP1;

		std::vector<VertexFormat> verticesP2;
		std::vector<VertexFormat> verticesC1;
		std::vector<VertexFormat> verticesC2;
		std::vector<VertexFormat> verticesC3;
		std::vector<VertexFormat> verticesC4;
		std::vector<VertexFormat> verticesE1;
		std::vector<VertexFormat> verticesE2;
		std::vector<VertexFormat> verticesE3;
		std::vector<VertexFormat> verticesE4;

		std::vector<unsigned int> indices;

		float offsetXZProp = 0.8, offsetYProp = 0.31;
		float offsetXZC = 0.8, offsetYC = 0.2;

		float angle = 0;


		Drone() {}
	};
}
