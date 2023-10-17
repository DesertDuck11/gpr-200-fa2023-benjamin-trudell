#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <dd11/camera.h> 

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void moveCamera(GLFWwindow* window, dd11::Camera* camera, dd11::CameraControls* controls);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];
dd11::Camera camera;
dd11::CameraControls cameraControls;

bool orbitting = false;

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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	//Camera Values
	camera.position = ew::Vec3(0.0f, 0.0f, 5.0f);
	camera.target = ew::Vec3(0.0f, 0.0f, 0.0f);
	camera.fov = 60.0f;
	camera.aspectRatio = (float)(SCREEN_WIDTH) / (float)(SCREEN_HEIGHT);
	camera.orthographic = false;
	camera.orthoSize = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		moveCamera(window, &camera, &cameraControls);

		//Set uniforms
		shader.use();
		shader.setMat4("_View", camera.ViewMatrix());
		shader.setMat4("_Projection", camera.ProjectionMatrix());

		//TODO: Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			ew::Mat4 modelMatrix = cubeTransforms[i].getModelMatrix();
			shader.setMat4("_Model", modelMatrix);

			//Draw cube
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Cubes"))
			{
				for (size_t i = 0; i < NUM_CUBES; i++)
				{
					ImGui::PushID(i);
					if (ImGui::CollapsingHeader("Transform")) {
						ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
						ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
						ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
					}
					ImGui::PopID();
				}
			}			
			if (ImGui::CollapsingHeader("Camera"))
			{
				ImGui::Checkbox("Orbit", &orbitting);
				if (orbitting)
				{
					camera.position.x = cosf((float)glfwGetTime()) * 5.0;
					camera.position.z = sinf((float)glfwGetTime()) * 5.0;
				}
				ImGui::DragFloat3("Position", &camera.position.x, 0.05f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.05f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic)
				{
					ImGui::DragFloat("Orthographic Height", &camera.orthoSize, 1.0f);
				}
				else
				{
					ImGui::DragFloat("FOV", &camera.fov, 1.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 1.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 1.0f);
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
	camera.aspectRatio = (float)(width) / (float)(height);
}

void moveCamera(GLFWwindow* window, dd11::Camera* camera, dd11::CameraControls* controls) {
	//If right mouse is not held, release cursor and return early.
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
		//Release cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
		return;
	}
	//GLFW_CURSOR_DISABLED hides the cursor, but the position will still be changed as we move our mouse.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Get screen mouse position this frame
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	//If we just started right clicking, set prevMouse values to current position.
	//This prevents a bug where the camera moves as soon as we click.
	if (controls->firstMouse) {
		controls->firstMouse = false;
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}

	//TODO: Get mouse position delta for this frame
	float deltaMouseX = mouseX - controls->prevMouseX;
	float deltaMouseY = mouseY - controls->prevMouseY;


	//TODO: Add to yaw and pitch
	controls->yaw += (deltaMouseX)*controls->mouseSensitivity;
	controls->pitch -= (deltaMouseY)*controls->mouseSensitivity;


	//TODO: Clamp pitch between -89 and 89 degrees
	if (controls->pitch > 89) 
	{
		controls->pitch = 89;
	}
	else if (controls->pitch < -89)
	{
		controls->pitch = -89;
	}
	
	//Remember previous mouse position
	controls->prevMouseX = mouseX;
	controls->prevMouseY = mouseY;

	controls->yaw *= dd11::CONVERT_TO_RADIANS;
	controls->pitch *= dd11::CONVERT_TO_RADIANS;

	//Construct forward vector using yaw and pitch. Don't forget to convert to radians!
	ew::Vec3 forward = ew::Vec3(
		cos(controls->yaw) * cos(controls->pitch), 
		sin(controls->pitch), 
		sin(controls->yaw) * cos(controls->pitch)
	);
	//By setting target to a point in front of the camera along its forward direction, our LookAt will be updated accordingly when rendering.
	camera->target = camera->position + forward;
}
