#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/camera.h"
#include "lab_m1/Tema2/Terrain.h"
#include "lab_m1/Tema2/Drone.h"
#include "lab_m1/Tema2/transform.h"
#include "lab_m1/Tema2/Obstacles.h"



namespace m1
{
	class Tema2 : public gfxc::SimpleScene
	{
	public:
		Tema2();
		~Tema2();

		void Init() override;
	private:
        void CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void RenderDrone(float deltaTime);

        //void GenerateObstaclesPos();
        void GenerateTree();
        void GenerateBuildings();
        bool CheckCollisions(float deltaTimeSeconds);
        void RenderTerrain();

    protected:
        implemented::Camera_H* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        float fov = RADIANS(60);
        float x = 10;
        float y = 10;
        float l = -10.0f;
        float r = 10.0f;
        float propellerRotation = 0;

        bool under = false;

        //glm::mat4 modelMatrix = glm::mat4(1);

        glm::vec3 dronePosition;

        std::vector<glm::vec3> treePositions;
        std::vector<glm::vec3> buildingPositions;

        Drone* drone = new Drone();
        Terrain* terrain = new Terrain(40, 40);
        Obstacles* obstacles = new Obstacles();
	};
}