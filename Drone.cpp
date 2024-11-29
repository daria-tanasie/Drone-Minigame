#include "lab_m1/Tema2/Drone.h"

#include <vector>

using namespace std;
using namespace m1;

void Drone::createDrone() {
    verticesP1 =
    {   // x e latime aici
        VertexFormat(glm::vec3(-0.1, -0.1,  0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1, -0.1,  0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1,  0.1,  0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1,  0.1,  0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1, -0.1, -0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1, -0.1, -0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1,  0.1, -0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1,  0.1, -0.9), glm::vec3(0,0,0)),
    };

    indicesP1 =
    {
        0, 1, 2,        1, 3, 2,
        2, 3, 7,        2, 7, 6,
        1, 7, 3,        1, 5, 7,
        6, 7, 4,        7, 5, 4,
        0, 4, 1,        1, 4, 5,
        2, 6, 4,        0, 2, 4,
    };

    verticesP2 =
    {   // z e latime aici
        VertexFormat(glm::vec3(-0.9, -0.1,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.9, -0.1,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.9,  0.1,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.9,  0.1,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.9, -0.1, -0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.9, -0.1, -0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.9,  0.1, -0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.9,  0.1, -0.1), glm::vec3(0,0,0)),
    };

    indicesP2 =
    {
        0, 1, 2,        1, 3, 2,
        2, 3, 7,        2, 7, 6,
        1, 7, 3,        1, 5, 7,
        6, 7, 4,        7, 5, 4,
        0, 4, 1,        1, 4, 5,
        2, 6, 4,        0, 2, 4,
    };

    verticesC1 =
    {   // cub de pe aripa dreapta cu z latime
        VertexFormat(glm::vec3(0.7, 0.1,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.9, 0.1,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.7,  0.3,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.9,  0.3,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.7, 0.1, -0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.9, 0.1, -0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.7,  0.3, -0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.9,  0.3, -0.1), glm::vec3(0,0,0)),
    };

    indicesC1 =
    {
        0, 1, 2,        1, 3, 2,
        2, 3, 7,        2, 7, 6,
        1, 7, 3,        1, 5, 7,
        6, 7, 4,        7, 5, 4,
        0, 4, 1,        1, 4, 5,
        2, 6, 4,        0, 2, 4,
    };

    verticesC2 =
    {   // cub de pe aripa stanga cu z latime
        VertexFormat(glm::vec3(-0.7, 0.1,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.9, 0.1,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.7,  0.3,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.9,  0.3,  0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.7, 0.1, -0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.9, 0.1, -0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.7,  0.3, -0.1), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.9,  0.3, -0.1), glm::vec3(0,0,0)),
    };

    indicesC2 =
    {
        0, 1, 2,        1, 3, 2,
        2, 3, 7,        2, 7, 6,
        1, 7, 3,        1, 5, 7,
        6, 7, 4,        7, 5, 4,
        0, 4, 1,        1, 4, 5,
        2, 6, 4,        0, 2, 4,
    };

    verticesC3 =
    {   // cub de pe aripa fata cu x latime
        VertexFormat(glm::vec3(0.1, 0.1,  -0.7), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1, 0.1,  -0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1,  0.3,  -0.7), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1,  0.3,  -0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1, 0.1, -0.7), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1, 0.1, -0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1,  0.3, -0.7), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1,  0.3, -0.9), glm::vec3(0,0,0)),
    };

    indicesC3 =
    {
        0, 1, 2,        1, 3, 2,
        2, 3, 7,        2, 7, 6,
        1, 7, 3,        1, 5, 7,
        6, 7, 4,        7, 5, 4,
        0, 4, 1,        1, 4, 5,
        2, 6, 4,        0, 2, 4,
    };

    verticesC4 =
    {   // cub de pe aripa spate cu x latime
         VertexFormat(glm::vec3(0.1, 0.1, 0.7), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1, 0.1, 0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1,  0.3, 0.7), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(0.1,  0.3, 0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1, 0.1, 0.7), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1, 0.1, 0.9), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1,  0.3, 0.7), glm::vec3(0,0,0)),
        VertexFormat(glm::vec3(-0.1,  0.3, 0.9), glm::vec3(0,0,0)),
    };

    indicesC4 =
    {
        0, 1, 2,        1, 3, 2,
        2, 3, 7,        2, 7, 6,
        1, 7, 3,        1, 5, 7,
        6, 7, 4,        7, 5, 4,
        0, 4, 1,        1, 4, 5,
        2, 6, 4,        0, 2, 4,
    };
}