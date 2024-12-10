#include "lab_m1/Tema2/Drone.h"

#include <vector>

using namespace std;
using namespace m1;

void Drone::createDrone() {
    verticesP1 =
    {   // paralelipiped
        VertexFormat(glm::vec3(-0.1, -0.1,  0.9), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(0.1, -0.1,  0.9), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(-0.1,  0.1,  0.9), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(0.1,  0.1,  0.9), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(-0.1, -0.1, -0.9), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(0.1, -0.1, -0.9), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(-0.1,  0.1, -0.9), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(0.1,  0.1, -0.9), glm::vec3(0.6f, 0.65f, 0.65f)),
    };

    verticesC1 =
    {   // cub aripa
        VertexFormat(glm::vec3(-0.1, -0.1,  0.1), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(0.1, -0.1,  0.1), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(-0.1, 0.1,  0.1), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(0.1, 0.1,  0.1), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(-0.1, -0.1, -0.1), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(0.1, -0.1, -0.1), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(-0.1, 0.1, -0.1), glm::vec3(0.6f, 0.65f, 0.65f)),
        VertexFormat(glm::vec3(0.1, 0.1, -0.1), glm::vec3(0.6f, 0.65f, 0.65f)),
    };

    verticesE1 =
    {   // elice
        VertexFormat(glm::vec3(-0.3, -0.01, 0.01), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(0.3, -0.01, 0.01), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(-0.3, 0.01, 0.01), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(0.3, 0.01, 0.01), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(-0.3, -0.01, -0.01), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(0.3, -0.01, -0.01), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(-0.3, 0.01, -0.01), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(0.3, 0.01, -0.01), glm::vec3(0, 0, 0)),
    };

    indices =
    {
        0, 1, 2,        1, 3, 2,
        2, 3, 7,        2, 7, 6,
        1, 7, 3,        1, 5, 7,
        6, 7, 4,        7, 5, 4,
        0, 4, 1,        1, 4, 5,
        2, 6, 4,        0, 2, 4,
    };
}

Mesh* Drone::GenerateTimer(glm::vec3 color) {
    glm::vec3 center = glm::vec3(0, 0, 0);
    float length = 1.5f;
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(center - glm::vec3(length / 2, 0, 0), color),
        VertexFormat(center + glm::vec3(length / 2, 0, 0), color),
        VertexFormat(center + glm::vec3(length / 2, length / 2, 0), color),
        VertexFormat(center + glm::vec3(-length / 2, length / 2, 0), color)
    };

    Mesh* square = new Mesh("square");
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };
    indices.push_back(0);
    indices.push_back(2);
    square->InitFromData(vertices, indices);
    return square;
}