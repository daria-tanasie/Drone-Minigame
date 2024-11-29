#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/Terrain.h"
#include "lab_m1/Tema2/Drone.h"

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
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }


    projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f);

    Terrain* terrain = new Terrain(24, 24);
    terrain->createTerrain();
    CreateMesh("terrain", terrain->vertices, terrain->indices);

    Drone* drone = new Drone();
    drone->createDrone();
    CreateMesh("droneP1", drone->verticesP1, drone->indicesP1);
    CreateMesh("droneP2", drone->verticesP2, drone->indicesP2);
    CreateMesh("droneC1", drone->verticesC1, drone->indicesC1);
    CreateMesh("droneC2", drone->verticesC2, drone->indicesC2);
    CreateMesh("droneC3", drone->verticesC3, drone->indicesC3);
    CreateMesh("droneC4", drone->verticesC4, drone->indicesC4);

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
}


void Tema2::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        RenderMesh(meshes["terrain"], shaders["TerrainShader"],modelMatrix);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        //modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        RenderMesh(meshes["droneP1"], shaders["DroneShader"], modelMatrix);
        RenderMesh(meshes["droneP2"], shaders["DroneShader"], modelMatrix);
        RenderMesh(meshes["droneC1"], shaders["DroneShader"], modelMatrix);
        RenderMesh(meshes["droneC2"], shaders["DroneShader"], modelMatrix);
        RenderMesh(meshes["droneC3"], shaders["DroneShader"], modelMatrix);
        RenderMesh(meshes["droneC4"], shaders["DroneShader"], modelMatrix);
    }

    if (renderCameraTarget)
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }
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
        glUniform3f(color2Loc, 0.45f, 0.29f, 0.09f);
    }

    if (!strcmp(shader->GetName(), "DroneShader")) {
        auto droneColorLoc = glGetUniformLocation(shader->GetProgramID(), "droneCol");
        glUniform3f(droneColorLoc, 0.6f, 0.65f, 0.65);
    }

    mesh->Render();
}

void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 2.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            // Translate the camera forward
            camera->TranslateForward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // Translate the camera to the left
            camera->TranslateRight(-(cameraSpeed * deltaTime));
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // Translate the camera backward
            camera->TranslateForward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // Translate the camera to the right
            camera->TranslateRight(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // Translate the camera downward
            camera->TranslateUpward(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // Translate the camera upward
            camera->TranslateUpward(cameraSpeed * deltaTime);
        }
    }

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

    if (key == GLFW_KEY_O) {
        projectionMatrix = glm::ortho(l, r, -10.0f, 10.0f, 0.1f, 100.0f);
    }

    if (key == GLFW_KEY_P) {
        projectionMatrix = glm::perspective(fov, 10.f, 0.1f, 100.0f);
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
