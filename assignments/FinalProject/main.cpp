#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

struct Light {
	ew::Vec3 position;
	ew::Vec3 color;
};

struct Material {
	float ambientK; //Ambient coefficient (0-1)
	float diffuseK; //Diffuse coefficient (0-1)
	float specular; //Specular coefficient (0-1)
	float shininess; //Shininess
};

const int MAX_LIGHTS = 4;
bool dayNight = false;

float SkyColor[4][3] = {
	{ 1.0f, 0.5f, 0.0f },
	{ 0.8f, 1.0f, 0.8f },
	{ 0.5f, 0.0f, 0.5f },
	{ 0.0f, 0.0f, 0.5f }
};

float SunColor[2][3] = {
	{ 1.0f, 1.0f, 1.0f },
	{ 0.8f, 0.0f, 0.8f }
};

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;


int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader lightShader("assets/unlit.vert", "assets/unlit.frag");
	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	ew::Shader skybox("assets/skybox.vert", "assets/skybox.frag");
	ew::Shader terrain("assets/terrain.vert", "assets/terrain.frag");

	unsigned int grassTexture = ew::loadTexture("assets/grass.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int rockTexture = ew::loadTexture("assets/rock.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int snowTexture = ew::loadTexture("assets/snow.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int heightMap = ew::loadTexture("assets/terrain.png", GL_REPEAT, GL_LINEAR);

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));
	//Skybox mesh
	ew::Mesh skyboxMesh(ew::createSphere(50.0f, 64));
	//Terrain mesh
	ew::Mesh terrainMesh = (ew::createPlane(100.f,100.f,512));
	ew::Mesh extraTerrain = (ew::createPlane(100.f, 100.f, 512));

	ew::Mesh lightMesh[MAX_LIGHTS];

	for (int i = 0; i < 4; i++)
	{
		lightMesh[i] = ew::createSphere(0.3f, 15);
	}

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	//Skybox transform
	ew::Transform skyboxTransform;
	ew::Transform terrainTransform;
	ew::Transform extraTerrainTransform[8];

	ew::Transform lightTransform[4];
	
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);
	//Skybox position
	skyboxTransform.position = ew::Vec3(0.0f, 0.0f, 0.0f);
	terrainTransform.position = ew::Vec3(0.0f, -10.05f, 0.0f);
	terrainTransform.rotation = ew::Vec3(0.0f, 180.f, 0.0f);
	extraTerrainTransform[0].position = ew::Vec3(99.0f, -10.0f, 0.0f);
	extraTerrainTransform[1].position = ew::Vec3(99.0f, -10.0f, 99.0f);
	extraTerrainTransform[2].position = ew::Vec3(99.0f, -10.0f, -99.0f);
	extraTerrainTransform[3].position = ew::Vec3(0.0f, -10.0f, -99.0f);
	extraTerrainTransform[4].position = ew::Vec3(0.0f, -10.0f, 99.0f);
	extraTerrainTransform[5].position = ew::Vec3(-99.0f, -10.0f, 0.0f);
	extraTerrainTransform[6].position = ew::Vec3(-99.0f, -10.0f, 99.0f);
	extraTerrainTransform[7].position = ew::Vec3(-99.0f, -10.0f, -99.0f);

	Light lights[MAX_LIGHTS];

	lightTransform[0].position = ew::Vec3(-5000.0, 0.0, 0.0);
	lights[0].position = lightTransform[0].position;
	lights[0].color = ew::Vec3(SunColor[0][0], SunColor[0][1], SunColor[0][2]);

	lights[1].color = ew::Vec3(0.6, 0.0, 1.0);
	lights[2].color = ew::Vec3(1.0, 0.0, 0.0);
	lights[3].color = ew::Vec3(0.0, 0.0, 1.0);

	Material material;

	material.shininess = 50.0f;
	material.ambientK = 0.2;
	material.diffuseK = 0.5;
	material.specular = 1.0;

	int numLights = 1;
	float lightIntensity = 1.0;
	bool orbit = false;
	int timeVar;

	resetCamera(camera, cameraController);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		if (orbit)
		{
			for (int i = 0; i < MAX_LIGHTS; i++)
			{
				lightTransform[i].position.x = cos(time + (2 * ew::PI / (numLights)*i)) * 3;
				lightTransform[i].position.z = (sin(time + (2 * ew::PI / (numLights)*i))) * 3;
				lights[i].position = lightTransform[i].position;
			}
		}

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);
		if(camera.other)
		{
			camera.position = ew::Vec3(camera.dist + 100.0, 7.0, 4.0);
			camera.target = camera.position + ew::Vec3(-1.0, 0.0, 0.0);
		}

		//RENDER
		glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		for (int i = 0; i < numLights; i++)
		{
			shader.setVec3("_Lights[" + std::to_string(i) + "].position", lights[i].position);
			shader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
		}

		shader.setVec3("camPos", camera.position);

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		//cubeMesh.draw();

		for (int i = 0; i < 8; i++)
		{
			shader.setMat4("_Model", extraTerrainTransform[i].getModelMatrix());
			extraTerrain.draw();
		}

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		//sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		//cylinderMesh.draw();

		shader.setFloat("shininess", material.shininess);
		shader.setFloat("ambient", material.ambientK);
		if (dayNight == true)
		{
			shader.setFloat("ambient", material.ambientK);
		}
		else
		{
			shader.setFloat("ambient", material.ambientK * 3);
		}
		shader.setFloat("specular", material.specular);
		shader.setFloat("diffuse", material.diffuseK);
		shader.setInt("numLights", numLights);
		shader.setFloat("lightIntensity", lightIntensity);

		//Terrain Shader
		terrain.use();

		terrain.setVec3("camPos", camera.position);
		terrain.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		terrain.setMat4("_Model", terrainTransform.getModelMatrix());
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, grassTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, rockTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, snowTexture);
		terrain.setInt("_HeightMap", 0);
		terrain.setInt("_GrassTexture", 1);
		terrain.setInt("_RockTexture", 2);
		terrain.setInt("_SnowTexture", 3);

		for (int i = 0; i < numLights; i++)
		{
			terrain.setVec3("_Lights[" + std::to_string(i) + "].position", lights[i].position);
			terrain.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
		}

		terrain.setFloat("shininess", material.shininess);
		if (dayNight == true)
		{
			terrain.setFloat("ambient", material.ambientK);
		}
		else
		{
			terrain.setFloat("ambient", material.ambientK * 3);
		}
		terrain.setFloat("specular", material.specular);
		terrain.setFloat("diffuse", material.diffuseK);
		terrain.setInt("numLights", numLights);
		terrain.setFloat("lightIntensity", lightIntensity);

		terrainMesh.draw();

		//TODO: Render point lights

		lightShader.use();

		for (int i = 0; i < numLights; i++)
		{
			lightShader.setMat4("_Model", lightTransform[i].getModelMatrix());
			lightShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
			//lightShader.setMat4("_ViewProjection", ew::Perspective(ew::Radians(60), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.01f, 100.0f)*
				//ew::LookAt(ew::Vec3(0.0), ew::Normalize(camera.target - camera.position), ew::Vec3(0, 1, 0)));
			lightShader.setVec3("_Color", lights[i].color);
			lightMesh[i].draw();
		}

		

		//Skyboix shader
		glDisable(GL_CULL_FACE);
		skybox.use();
		//skybox.setMat4("_ViewProjection", camera.ProjectionMatrix()* camera.ViewMatrix());
		skybox.setMat4("_ViewProjection", ew::Perspective(ew::Radians(60), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.01f, 100.0f) *
											ew::LookAt(ew::Vec3(0.0), ew::Normalize(camera.target - camera.position), ew::Vec3(0, 1, 0)));
		skybox.setMat4("_Model", skyboxTransform.getModelMatrix());
		skybox.setVec3Array("skyColor", 4, SkyColor);
		skybox.setVec3Array("sunColor", 2, SunColor);
		if (dayNight == true)
		{
			skybox.setInt("dayNight", 1);
		}
		else
		{
			skybox.setInt("dayNight", 0);
		}
		skyboxMesh.draw();
		glEnable(GL_CULL_FACE);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				//ImGui::Checkbox("Other Mode", &camera.other);
				if (ImGui::Button("Toggle Transition"))
				{
					timeVar = glfwGetTime();
					camera.other = true;
				}
				if (!camera.other)
				{
					ImGui::Checkbox("Orthographic", &camera.orthographic);
					if (camera.orthographic) {
						ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
					}
					else {
						ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
					}
				}
				else
				{
					ImGui::SliderFloat("Distance", &camera.dist, 0.0f, (6 * 180.0f));
					if(((glfwGetTime() - timeVar) / 20) < 1.0f)
						camera.dist = ew::lerp(0, 6 * 180, 1 - (glfwGetTime() - timeVar) / 20);
				}
				
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			if (ImGui::CollapsingHeader("Light Settings"))
			{
				ImGui::DragFloat("Ambient", &material.ambientK, 0.01, 0.0, 1.0);
				ImGui::DragFloat("Diffuse", &material.diffuseK, 0.01, 0.0, 1.0);
				ImGui::DragFloat("Specular", &material.specular, 0.01, 0.0, 1.0);
				ImGui::DragFloat("Shininess", &material.shininess, 16.0, 0.0, 1024);
				ImGui::DragFloat("Light Intensity", &lightIntensity, 0.01, 0.0, 1.0f);
			}

			if (ImGui::CollapsingHeader("Skybox Settings"))
			{
				ImGui::Checkbox("Day/Night", &dayNight);
				if (dayNight == true)
				{
					lights[0].color = ew::Vec3(SunColor[1][0], SunColor[1][1], SunColor[1][2]);
				}
				else
				{
					lights[0].color = ew::Vec3(SunColor[0][0], SunColor[0][1], SunColor[0][2]);
				}
				ImGui::ColorEdit3("Day Sun Color", SunColor[0], 1.0f);
				ImGui::ColorEdit3("Day Sky Color 1", SkyColor[0], 1.0f);
				ImGui::ColorEdit3("Day Sky Color 2", SkyColor[1], 1.0f);
				ImGui::ColorEdit3("Night Sun Color", SunColor[1], 1.0f);
				ImGui::ColorEdit3("Night Sky Color 1", SkyColor[2], 1.0f);
				ImGui::ColorEdit3("Night Sky Color 2", SkyColor[3], 1.0f);
			}

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 80);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 150.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}

