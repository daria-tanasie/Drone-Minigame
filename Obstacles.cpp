#include "lab_m1/Tema2/Obstacles.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;

double distance(glm::vec3 pos, glm::vec3 current) {
    return sqrt(pow((current.x - pos.z), 2) + pow((current.z - pos.z), 2));
}

void Obstacles::GenerateObstaclesPos()
{
    int i = 0;

    while (i < 5) {
        glm::vec3 position = glm::vec3((rand() % 61 - 30), (rand() % 7 + 3), (rand() % 61 - 30));
        bool okPos = true;

        for (int j = 0; j < treePositions.size(); j++) {
            if (distance(treePositions[j], position) < 7.0f) {
                okPos = false;
                break;
            }
        }

        if (okPos) {
            treePositions.push_back(position);
            i++;
        }
    }

    i = 0;
    while (i < 5) {
        glm::vec3 position = glm::vec3((rand() % 61 - 30), 0, (rand() % 61 - 30));
        bool okPos = true;

        for (int j = 0; j < treePositions.size(); j++) {
            if (distance(treePositions[j], position) < 7.0f) {
                okPos = false;
                break;
            }
        }

        if (okPos) {
            for (int k = 0; k < buildingPositions.size(); k++) {
                if (distance(buildingPositions[k], position) < 9.0f) {
                    okPos = false;
                    break;
                }
            }
        }

        if (okPos) {
            buildingPositions.push_back(position);
            i++;
        }
    }
}

