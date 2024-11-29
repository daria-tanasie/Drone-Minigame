#pragma once

#include "lab_m1/Tema2/Tema2.h"
#include "components/simple_scene.h"
#include "lab_m1/Tema2/Terrain.h"
#include <vector>

namespace m1
{
	class Drone : public gfxc::SimpleScene
	{
	public:

		void Drone::createDrone();

		std::vector<VertexFormat> verticesP1;
		std::vector<unsigned int> indicesP1;

		std::vector<VertexFormat> verticesP2;
		std::vector<unsigned int> indicesP2;

		std::vector<VertexFormat> verticesC1;
		std::vector<unsigned int> indicesC1;

		std::vector<VertexFormat> verticesC2;
		std::vector<unsigned int> indicesC2;

		std::vector<VertexFormat> verticesC3;
		std::vector<unsigned int> indicesC3;

		std::vector<VertexFormat> verticesC4;
		std::vector<unsigned int> indicesC4;


		Drone() {}
	};
}
