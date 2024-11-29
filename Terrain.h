#pragma once

#include "lab_m1/Tema2/Tema2.h"
#include "components/simple_scene.h"
#include <vector>


namespace m1
{
	class Terrain : public gfxc::SimpleScene
	{
	public:

		void Terrain::createTerrain();

		int len, width;
		std::vector<VertexFormat> vertices;
		std::vector<unsigned int> indices;

		Terrain(int n, int m)
			: len(n), width(m) {}
	};
}