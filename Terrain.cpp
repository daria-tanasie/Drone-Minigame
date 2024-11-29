#include "lab_m1/Tema2/Terrain.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;

void Terrain::createTerrain() {
    {
        unsigned int topL, topR, bottomL, bottomR;

        for (int i = 0; i < len; i++) {
            for (int j = 0; j < width; j++) {

                glm::vec3 pos = glm::vec3(i * (1.2f), 0, j);
                glm::vec3 col = glm::vec3(0.0f, 0.1f, 0.0f);
                vertices.push_back(VertexFormat(pos, col));
            }
        }

        for (int i = 0; i < len - 1; i++) {
            for (int j = 0; j < width - 1; j++) {
                topL = i * len + j;
                topR = topL + 1;
                bottomL = (i + 1) * len + j;
                bottomR = bottomL + 1;

                indices.push_back(topL);
                indices.push_back(bottomL);
                indices.push_back(topR);

                indices.push_back(bottomL);
                indices.push_back(topR);
                indices.push_back(bottomR);
            }
        }
    }
}