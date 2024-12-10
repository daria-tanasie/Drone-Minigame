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
    camera->Set(glm::vec3(0, 4, 7.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    secondCamera = new implemented::Camera_H();
    secondCamera->Set(glm::vec3(0, 4, 7.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    if (isMultiPlayer) {
        secondDrone->createDrone();
    }

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

    {
        meshes["squareBig"] = obstacles->GenerateCheckPoint(glm::vec3(1, 1, 1));
        meshes["squareLilG"] = obstacles->GenerateCheckPoint(glm::vec3(0, 1, 0));
        meshes["squareLilR"] = obstacles->GenerateCheckPoint(glm::vec3(1, 0, 0));
    }

    {
        meshes["timer"] = drone->GenerateTimer(glm::vec3(0.8f, 0.6f, 0.1f));
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

   obstacles->GenerateObstaclesPos();
   treePositions = obstacles->treePositions;
   buildingPositions = obstacles->buildingPositions;
   gatePositions = obstacles->gatePositions;

   drone->checked.push_back(1);
   secondDrone->checked.push_back(1);
   for (int i = 1; i < gatePositions.size(); i++) {
       drone->checked.push_back(0);
       if (isMultiPlayer) {
           secondDrone->checked.push_back(0);
       }
   }
}


void Tema2::FrameStart()
{
    glClearColor(0.14f, 0.89f, 0.9f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int split = 1;

    if (isMultiPlayer) {
        split = 2;
    }

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x / split, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
    RenderTerrain();

    if (drone->checksPassed < 5 && !isMultiPlayer) {
        time = time - 0.005 * deltaTimeSeconds;
    }
    
    if (time > 0) {
        RenderDrone(deltaTimeSeconds, camera, dronePosition, drone);
        dronePosition = camera->GetTargetPosition();
    }

    GenerateTree();
    GenerateBuildings();
    GenerateChecks(drone);
    CheckCollisions(deltaTimeSeconds, dronePosition, camera, drone);

    if (isMultiPlayer) {
        player = 2;
        glm::ivec2 resolution = window->GetResolution();
        glViewport(resolution.x / 2, 0, resolution.x / 2, resolution.y);
        RenderTerrain();
        RenderDrone(deltaTimeSeconds, secondCamera, dronePosition2, secondDrone);
        dronePosition2 = secondCamera->GetTargetPosition();
        GenerateTree();
        GenerateBuildings();
        GenerateChecks(secondDrone);
        CheckCollisions(deltaTimeSeconds, dronePosition2, secondCamera, secondDrone);
    }
    player = 1;
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

    if (player == 1) {
        glUseProgram(shader->program);
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }
    else {
        glUseProgram(shader->program);
        glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(secondCamera->GetViewMatrix()));
        glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }

    

    if (!strcmp(shader->GetName(),"TerrainShader")) {

        auto frecvLoc = glGetUniformLocation(shader->GetProgramID(), "frecv");
        glUniform1f(frecvLoc, 0.5f);

        auto color1Loc = glGetUniformLocation(shader->GetProgramID(), "color1");
        glUniform3f(color1Loc, 0.57f, 0.87f, 0.5f);

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
    //DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 5.0f, backDraft = 0.05;
    if (window->KeyHold(GLFW_KEY_W)) {
        if (CheckCollisions(deltaTime, dronePosition, camera, drone)) {
            camera->MoveForward(-cameraSpeed * backDraft * 0.2);
        }
        else {
            camera->MoveForward(cameraSpeed * deltaTime);
        }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        if (CheckCollisions(deltaTime, dronePosition, camera, drone)) {
            camera->MoveRight((cameraSpeed * backDraft));
        }
        else {
            camera->MoveRight(-(cameraSpeed * deltaTime));
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        if (!CheckCollisions(deltaTime, dronePosition, camera, drone)) {
            camera->MoveForward(-(cameraSpeed * deltaTime));
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        if (CheckCollisions(deltaTime, dronePosition, camera, drone)) {
            camera->MoveRight(-(cameraSpeed * backDraft));
        }
        else {
            camera->MoveRight(cameraSpeed * deltaTime);
        }
    }

    if (window->KeyHold(GLFW_KEY_F) && !under) {
        if (CheckCollisions(deltaTime, dronePosition, camera, drone)) {
            camera->TranslateUpward(cameraSpeed * backDraft);
        }
        else {
            camera->TranslateUpward(-cameraSpeed * deltaTime);
        }
    }

    if (window->KeyHold(GLFW_KEY_R)) {
        if (CheckCollisions(deltaTime, dronePosition, camera, drone)) {
            camera->TranslateUpward(-cameraSpeed * backDraft);
        }
        else {
            camera->TranslateUpward(cameraSpeed * deltaTime);
        }
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        drone->angle += deltaTime * RADIANS(50);
        camera->RotateThirdPerson_OY(RADIANS(50) * deltaTime);
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        drone->angle += deltaTime * -RADIANS(50);
        camera->RotateThirdPerson_OY(-RADIANS(50) * deltaTime);
    }

    // -------------------------------------------------------

    if (isMultiPlayer) {

        if (window->KeyHold(GLFW_KEY_I)) {
            if (CheckCollisions(deltaTime, dronePosition2, secondCamera, secondDrone)) {
                secondCamera->MoveForward(-cameraSpeed * backDraft * 0.2);
            }
            else {
                secondCamera->MoveForward(cameraSpeed * deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_J)) {
            if (CheckCollisions(deltaTime, dronePosition2, secondCamera, secondDrone)) {
                secondCamera->MoveRight((cameraSpeed * backDraft));
            }
            else {
                secondCamera->MoveRight(-(cameraSpeed * deltaTime));
            }
        }

        if (window->KeyHold(GLFW_KEY_K)) {
            if (!CheckCollisions(deltaTime, dronePosition2, secondCamera, secondDrone)) {
                secondCamera->MoveForward(-(cameraSpeed * deltaTime));
            }
        }

        if (window->KeyHold(GLFW_KEY_L)) {
            if (CheckCollisions(deltaTime, dronePosition2, secondCamera, secondDrone)) {
                secondCamera->MoveRight(-(cameraSpeed * backDraft));
            }
            else {
                secondCamera->MoveRight(cameraSpeed * deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_H) && !under) {
            if (CheckCollisions(deltaTime, dronePosition2, secondCamera, secondDrone)) {
                secondCamera->TranslateUpward(cameraSpeed * backDraft);
            }
            else {
                secondCamera->TranslateUpward(-cameraSpeed * deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_Y)) {
            if (CheckCollisions(deltaTime, dronePosition2, secondCamera, secondDrone)) {
                secondCamera->TranslateUpward(-cameraSpeed * backDraft);
            }
            else {
                secondCamera->TranslateUpward(cameraSpeed * deltaTime);
            }
        }

        if (window->KeyHold(GLFW_KEY_U)) {
            secondDrone->angle += deltaTime * RADIANS(50);
            secondCamera->RotateThirdPerson_OY(RADIANS(50) * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_O)) {
            secondDrone->angle += deltaTime * -RADIANS(50);
            secondCamera->RotateThirdPerson_OY(-RADIANS(50) * deltaTime);
        }
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }
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

void Tema2::RenderDrone(float deltaTime, implemented::Camera_H* cam, glm::vec3 dronePos, Drone* drone) {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, cam->GetTargetPosition());

    glm::mat4 paralMatrix1 = modelMatrix;
    paralMatrix1 *= transform::RotateOY(RADIANS(45));
    paralMatrix1 *= transform::RotateOY(drone->angle);
    paralMatrix1 *= transform::Scale(1, 0.25, 1);
    RenderMesh(meshes["droneP1"], shaders["DroneShader"], paralMatrix1);

    glm::mat4 paralMatrix2 = modelMatrix;
    paralMatrix2 *= transform::RotateOY(RADIANS(-45));
    paralMatrix2 *= transform::RotateOY(drone->angle);
    paralMatrix2 *= transform::Scale(1, 0.25, 1);
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

    if (drone->checksPassed < 5) {
        RenderArrow(modelMatrix, dronePos, drone);
    }

    if (!isMultiPlayer) {
        RenderTimer(modelMatrix, drone);
    }
}

void Tema2::RenderTimer(glm::mat4 modelMatrix, Drone* drone) {
    glm::mat4 timerMatrix = modelMatrix;
    timerMatrix *= transform::RotateOY(drone->angle);
    timerMatrix *= transform::Translate(0, 0.9, 0);
    timerMatrix *= transform::Scale(time, 0.1, 1);
    RenderMesh(meshes["timer"], shaders["VertexColor"], timerMatrix);
}

void Tema2::RenderArrow(glm::mat4 modelMatrix, glm::vec3 dronePos, Drone* drone) {
        glm::mat4 arrowMatrix = modelMatrix;
        glm::mat4 side1Matrix = modelMatrix;
        glm::mat4 side2Matrix = modelMatrix;
        glm::vec3 currentCheckPos = gatePositions[drone->currentGreen];
        float arrowAngle = atan2(currentCheckPos.x + 0.75f - dronePos.x, currentCheckPos.z - dronePos.z);
        arrowMatrix *= transform::RotateOY(arrowAngle);
        arrowMatrix *= transform::Translate(0, 0.5, 0);
        arrowMatrix *= transform::Scale(0.2, 0.2, 0.4);

        side1Matrix *= transform::RotateOY(arrowAngle);
        side1Matrix *= transform::Translate(-0.07, 0.5, 0.3);
        side1Matrix *= transform::Scale(0.1, 0.2, 0.1);
        side1Matrix *= transform::RotateOY(RADIANS(45));

        side2Matrix *= transform::RotateOY(arrowAngle);
        side2Matrix *= transform::Translate(0.07, 0.5, 0.3);
        side2Matrix *= transform::Scale(0.1, 0.2, 0.1);
        side2Matrix *= transform::RotateOY(RADIANS(-45));

        RenderMesh(meshes["droneP1"], shaders["VertexColor"], arrowMatrix);
        RenderMesh(meshes["droneP1"], shaders["VertexColor"], side1Matrix);
        RenderMesh(meshes["droneP1"], shaders["VertexColor"], side2Matrix);
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

void Tema2::GenerateChecks(Drone* drone) {
    for (int i = 0; i < gatePositions.size(); i++) {
        glm::mat4 checkMatrix = glm::mat4(1);
        checkMatrix *= transform::Translate(gatePositions[i].x, gatePositions[i].y, gatePositions[i].z);
        RenderMesh(meshes["squareBig"], shaders["VertexColor"], checkMatrix);

        glm::mat4 checkLilMatrix = glm::mat4(1);
        checkLilMatrix *= transform::Translate(gatePositions[i].x + 0.15, gatePositions[i].y - 0.15, gatePositions[i].z);
        checkLilMatrix *= transform::Scale(0.8, 0.8, 1);

        if (drone->checked[i] == 1) {
            RenderMesh(meshes["squareLilG"], shaders["VertexColor"], checkLilMatrix);
        }
        else {
            RenderMesh(meshes["squareLilR"], shaders["VertexColor"], checkLilMatrix);
        }
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

bool Tema2::CheckCollisions(float deltaTimeSeconds, glm::vec3 dronePos, implemented::Camera_H* cam, Drone* drone) {
    if (dronePos.y - 1.5f <= noise(glm::vec2(dronePos.x, dronePos.z))) {
        under = true;
        cam->TranslateUpward(3.0f * deltaTimeSeconds);
    }
    else {
        under = false;
    }

    for (int i = 0; i < buildingPositions.size(); i++) {
        if (dronePos.x <= buildingPositions[i].x + 3 && dronePos.x >= buildingPositions[i].x - 3
            && dronePos.z <= buildingPositions[i].z + 3 && dronePos.z >= buildingPositions[i].z - 3
            && dronePos.y <= buildingPositions[i].y + 10.3f) {
            return true;
            break;
        }

        if (dronePos.x <= buildingPositions[i].x + 3 && dronePos.x >= buildingPositions[i].x - 3
            && dronePos.z <= buildingPositions[i].z + 3 && dronePos.z >= buildingPositions[i].z - 3
            && dronePos.y <= buildingPositions[i].y + 10) {
            return true;
            break;
        }
    }

    for (int i = 0; i < treePositions.size(); i++) {
        if (dronePos.y <= treePositions[i].y + 1.5f && dronePos.y >= treePositions[i].y - 1.5f) {
            if (dronePos.x <= treePositions[i].x + 3 && dronePos.x >= treePositions[i].x - 3
                && dronePos.z <= treePositions[i].z + 3 && dronePos.z >= treePositions[i].z - 3)
                return true;
        }
        else if (dronePos.y <= treePositions[i].y - 1.5f) {
            if (dronePos.x <= treePositions[i].x + 1 && dronePos.x >= treePositions[i].x - 1
                && dronePos.z <= treePositions[i].z + 1 && dronePos.z >= treePositions[i].z - 1)
                return true;
        }
    }

    for (int i = 0; i < gatePositions.size(); i++) {
        if (i == drone->currentGreen) {
            if (dronePos.y <= gatePositions[i].y && dronePos.y >= gatePositions[i].y - 1.5f
                && dronePos.x >= gatePositions[i].x && dronePos.x <= gatePositions[i].x + 1.5f
                && dronePos.z >= gatePositions[i].z - 0.2f && dronePos.z <= gatePositions[i].z + 0.2f) {
                
                drone->checksPassed++;
                drone->checked[i] = 0;
                drone->currentGreen++;

                if (drone->checksPassed < 5) {
                    drone->checked[drone->currentGreen] = 1;
                }
            }
        }
    }

    return false;
}