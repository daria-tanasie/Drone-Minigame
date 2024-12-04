#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
    renderCameraTarget = false;

    camera = new implemented::Camera_H();
    camera->Set(glm::vec3(0, 3, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("trunk");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "props"), "oildrum.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("building");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }


    projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f);

    terrain->createTerrain();
    CreateMesh("terrain", terrain->vertices, terrain->indices);

    drone->createDrone();
    CreateMesh("droneP1", drone->verticesP1, drone->indices);
    CreateMesh("droneC1", drone->verticesC1, drone->indices);
    CreateMesh("droneE1", drone->verticesE1, drone->indices);

    {
        Shader* shader = new Shader("TerrainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderTerrain.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShaderTerrain.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("DroneShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderDrone.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShaderDrone.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("ObstaclesShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShaderObstacles.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShaderObstacles.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    GenerateObstaclesPos();
}


void Tema2::FrameStart()
{
    glClearColor(0.14f, 0.89f, 0.9f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
    RenderTerrain();
    
    RenderDrone(deltaTimeSeconds);
    dronePosition = camera->GetTargetPosition();
    
    GenerateTree();
    GenerateBuildings();

    CheckCollisions(deltaTimeSeconds);
}

void Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

    glBindVertexArray(0);

    CheckOpenGLError();

    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    glUseProgram(shader->program);
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    if (!strcmp(shader->GetName(),"TerrainShader")) {

        auto frecvLoc = glGetUniformLocation(shader->GetProgramID(), "frecv");
        glUniform1f(frecvLoc, 0.5f);

        auto color1Loc = glGetUniformLocation(shader->GetProgramID(), "color1");
        glUniform3f(color1Loc, 0.09f, 0.43f, 0.14f);

        auto color2Loc = glGetUniformLocation(shader->GetProgramID(), "color2");
        glUniform3f(color2Loc, 0.09f, 0.31f, 0.07f);
    }

    if (!strcmp(shader->GetName(), "ObstaclesShader")) {
        auto colorLoc = glGetUniformLocation(shader->GetProgramID(), "colorObs");
        
        if (!strcmp(mesh->GetMeshID(), "sphere")) {
            glUniform3f(colorLoc, 0.11f, 0.67f, 0.2f);
        }
        else if (!strcmp(mesh->GetMeshID(), "trunk")) {
            glUniform3f(colorLoc, 0.38f, 0.24f, 0.14f);
        }
        else if (!strcmp(mesh->GetMeshID(), "building")) {
            glUniform3f(colorLoc, 0.28f, 0.34f, 0.32f);
        }
    }

    mesh->Render();
}

void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    //if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    //{
        float cameraSpeed = 5.0f;
        if (window->KeyHold(GLFW_KEY_W)) {
            camera->MoveForward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            camera->MoveRight(-(cameraSpeed * deltaTime));
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            //camera->TranslateForward(-cameraSpeed * deltaTime);
            camera->MoveForward(-(cameraSpeed * deltaTime));
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            //camera->TranslateRight(cameraSpeed * deltaTime);
            camera->MoveRight(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_Q) && !underTer) {
            camera->TranslateUpward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera->TranslateUpward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_X)) {
            drone->angle += deltaTime * RADIANS(50);
            camera->RotateThirdPerson_OY(RADIANS(50) * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_C)) {
            drone->angle += deltaTime * -RADIANS(50);
            camera->RotateThirdPerson_OY(-RADIANS(50) * deltaTime);
        }
    //}

    if (window->KeyHold(GLFW_KEY_T)) {
        fov += RADIANS(5) * deltaTime;
        projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.1f, 100.0f);
    }

    if (window->KeyHold(GLFW_KEY_Y)) {
        fov -= RADIANS(5) * deltaTime;
        projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.1f, 100.0f);
    }

    if (window->KeyHold(GLFW_KEY_J)) {
        l -= 5 * deltaTime;
        r += 5 * deltaTime;
        projectionMatrix = glm::ortho(l, r, -10.0f, 10.0f, 0.1f, 100.0f);
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }
    // TODO(student): Switch projections

    /*if (key == GLFW_KEY_O) {
        projectionMatrix = glm::ortho(l, r, -10.0f, 10.0f, 0.1f, 100.0f);
    }

    if (key == GLFW_KEY_P) {
        projectionMatrix = glm::perspective(fov, 10.f, 0.1f, 100.0f);
    }*/
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            // Rotate the camera in first-person
            camera->RotateFirstPerson_OX(-1 * sensivityOX * deltaY);
            camera->RotateFirstPerson_OY(-1 * sensivityOY * deltaX);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            // Rotate the camera in third-person
            camera->RotateThirdPerson_OX(sensivityOX * deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * deltaX);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

void Tema2::RenderTerrain()
{
    glm::mat4 terrainMatrix = glm::mat4(1);
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], terrainMatrix);
    terrainMatrix *= transform::Translate(-40, 0, -37);
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], terrainMatrix);
    terrainMatrix = glm::mat4(1);
    terrainMatrix *= transform::Translate(0, 0, -37);
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], terrainMatrix);
    terrainMatrix = glm::mat4(1);
    terrainMatrix *= transform::Translate(-38, 0, 0);
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], terrainMatrix);
}

void Tema2::RenderDrone(float deltaTime) {
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());

        glm::mat4 paralMatrix1 = modelMatrix;
        paralMatrix1 *= transform::RotateOY(RADIANS(45));
        paralMatrix1 *= transform::RotateOY(drone->angle);
        RenderMesh(meshes["droneP1"], shaders["DroneShader"], paralMatrix1);

        glm::mat4 paralMatrix2 = modelMatrix;
        paralMatrix2 *= transform::RotateOY(RADIANS(-45));
        paralMatrix2 *= transform::RotateOY(drone->angle);
        RenderMesh(meshes["droneP1"], shaders["DroneShader"], paralMatrix2);

        glm::mat4 cubeMatrix1 = modelMatrix;
        cubeMatrix1 *= transform::RotateOY(RADIANS(45));
        cubeMatrix1 *= transform::RotateOY(drone->angle);
        cubeMatrix1 *= transform::Translate(drone->offsetXZC, drone->offsetYC, 0);
        RenderMesh(meshes["droneC1"], shaders["DroneShader"], cubeMatrix1);

        glm::mat4 cubeMatrix2 = modelMatrix;
        cubeMatrix2 *= transform::RotateOY(RADIANS(45));
        cubeMatrix2 *= transform::RotateOY(drone->angle);
        cubeMatrix2 *= transform::Translate(-drone->offsetXZC, drone->offsetYC, 0);
        RenderMesh(meshes["droneC1"], shaders["DroneShader"], cubeMatrix2);

        glm::mat4 cubeMatrix3 = modelMatrix;
        cubeMatrix3 *= transform::RotateOY(RADIANS(45));
        cubeMatrix3 *= transform::RotateOY(drone->angle);
        cubeMatrix3 *= transform::Translate(0, drone->offsetYC, drone->offsetXZC);
        RenderMesh(meshes["droneC1"], shaders["DroneShader"], cubeMatrix3);

        glm::mat4 cubeMatrix4 = modelMatrix;
        cubeMatrix4 *= transform::RotateOY(RADIANS(45));
        cubeMatrix4 *= transform::RotateOY(drone->angle);
        cubeMatrix4 *= transform::Translate(0, drone->offsetYC, -drone->offsetXZC);
        RenderMesh(meshes["droneC1"], shaders["DroneShader"], cubeMatrix4);

        propellerRotation += deltaTime * 10.0f;

        glm::mat4 propellerMatrix1 = modelMatrix;
        propellerMatrix1 *= transform::RotateOY(RADIANS(45));
        propellerMatrix1 *= transform::RotateOY(drone->angle);
        propellerMatrix1 *= transform::Translate(drone->offsetXZProp, drone->offsetYProp, 0);
        propellerMatrix1 *= transform::RotateOY(propellerRotation);
        RenderMesh(meshes["droneE1"], shaders["DroneShader"], propellerMatrix1);

        glm::mat4 propellerMatrix2 = modelMatrix;
        propellerMatrix2 *= transform::RotateOY(RADIANS(45));
        propellerMatrix2 *= transform::RotateOY(drone->angle);
        propellerMatrix2 *= transform::Translate(-drone->offsetXZProp, drone->offsetYProp, 0);
        propellerMatrix2 *= transform::RotateOY(propellerRotation);

        RenderMesh(meshes["droneE1"], shaders["DroneShader"], propellerMatrix2);

        glm::mat4 propellerMatrix3 = modelMatrix;
        propellerMatrix3 *= transform::RotateOY(RADIANS(45));
        propellerMatrix3 *= transform::RotateOY(drone->angle);
        propellerMatrix3 *= transform::Translate(0, drone->offsetYProp, drone->offsetXZProp);
        propellerMatrix3 *= transform::RotateOY(propellerRotation);
        RenderMesh(meshes["droneE1"], shaders["DroneShader"], propellerMatrix3);

        glm::mat4 propellerMatrix4 = modelMatrix;
        propellerMatrix4 *= transform::RotateOY(RADIANS(45));
        propellerMatrix4 *= transform::RotateOY(drone->angle);
        propellerMatrix4 *= transform::Translate(0, drone->offsetYProp, -drone->offsetXZProp);
        propellerMatrix4 *= transform::RotateOY(propellerRotation);
        RenderMesh(meshes["droneE1"], shaders["DroneShader"], propellerMatrix4);

    }
}

void Tema2::GenerateObstaclesPos()
{
    for (int i = 0; i < 5; i++) {
        glm::vec3 position = glm::vec3((rand() % 55 - 27), (rand() % 7 + 3), (rand() % 55 - 27));

        if (i == 0) {
            treePositions.push_back(position);
        }

        for (int j = 0; j < treePositions.size(); j++) {
            if (abs(position.x - treePositions[j].x) > 7 || abs(position.z - treePositions[j].z) > 7) {
                treePositions.push_back(position);
            }
            else {
                break;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        glm::vec3 position = glm::vec3((rand() % 55 - 27), 0, (rand() % 55 - 27));

        for (int j = 0; j < treePositions.size(); j++) {
            if ((abs(position.x - treePositions[j].x) > 7 || abs(position.z - treePositions[j].z) > 7)
                && i == 0) {
                buildingPositions.push_back(position);
            }
            else if ((abs(position.x - treePositions[j].x) > 7 || abs(position.z - treePositions[j].z) > 7)) {
                for (int k = 0; k < buildingPositions.size(); k++) {
                    if (abs(position.x - buildingPositions[k].x) > 7 || abs(position.z - buildingPositions[k].z) > 7) {
                        buildingPositions.push_back(position);
                    }
                    else {
                        break;
                    }
                }
            }
            else {
                break;
            }
        }
    }
}

void Tema2::GenerateBuildings()
{
    for (int i = 0; i < buildingPositions.size(); i++) {
        glm::mat4 buildingMatrix = glm::mat4(1);
        buildingMatrix *= transform::Translate(buildingPositions[i].x, buildingPositions[i].y, buildingPositions[i].z);
        buildingMatrix *= transform::Scale(3, 20, 3);
        RenderMesh(meshes["building"], shaders["ObstaclesShader"], buildingMatrix);
    }
}

void Tema2::GenerateTree()
{
    for (int i = 0; i < treePositions.size(); i++) {
        glm::mat4 leavesMatrix = glm::mat4(1);
        leavesMatrix *= transform::Translate(treePositions[i].x, treePositions[i].y, treePositions[i].z);
        leavesMatrix *= transform::Scale(4, 3, 4);
        RenderMesh(meshes["sphere"], shaders["ObstaclesShader"], leavesMatrix);

        glm::mat4 trunkMatrix = glm::mat4(1);
        trunkMatrix *= transform::Translate(treePositions[i].x, treePositions[i].y, treePositions[i].z);
        trunkMatrix *= transform::Scale(1, -treePositions[i].y, 1);
        RenderMesh(meshes["trunk"], shaders["ObstaclesShader"], trunkMatrix);
    }
}

float random(glm::vec2 st) {
    float nr = (sin(dot(st,
        glm::vec2(12.9898, 78.233)))
        * 43758.5453123);
    return (nr - long(nr));
}

float noise(glm::vec2 st) {
    glm::vec2 i = floor(st);
    glm::vec2 f = fract(st);

    float a = random(i);
    float b = random(i + glm::vec2(1.0, 0.0));
    float c = random(i + glm::vec2(0.0, 1.0));
    float d = random(i + glm::vec2(1.0, 1.0));

    glm::vec2 u = f * f * (3.0f - 2.0f * f);
    float mixed = (a * (1.0f - u.x)) + (b * u.x) +
        (c - a) * u.y * (1.0 - u.x) +
        (d - b) * u.x * u.y;

    return mixed;
}

void Tema2::CheckCollisions(float deltaTimeSeconds) {
    if (dronePosition.y - 1.5f <= noise(glm::vec2(dronePosition.x, dronePosition.z))) {
        underTer = true;
        camera->TranslateUpward(3.0f * deltaTimeSeconds);
    }
    else {
        underTer = false;
    }
}