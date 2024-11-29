#pragma once

#include "components/simple_scene.h"
#include <vector>

namespace m1
{
	class Terrain : public gfxc::SimpleScene
	{
	public:

		void createTerrain();

		int len, width;
		std::vector<VertexFormat> vertices;
		std::vector<unsigned int> indices;

		Terrain(int n, int m) {
			len = n;
			width = m;
		}
	};
}