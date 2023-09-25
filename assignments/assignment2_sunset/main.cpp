#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <dd11/shader.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

struct Vertex {
	float x, y, z;
	float u, v;
};
Vertex vertices[4] = {
	//x    y    z    u    v
   { -1.0, -1.0, 0.0, 0.0, 0.0 }, //Bottom left
   {  1.0, -1.0, 0.0, 1.0, 0.0 }, //Bottom right
   {  1.0,  1.0, 0.0, 1.0, 1.0 },  //Top right
   { -1.0,  1.0, 0.0, 0.0, 1.0 }  //Top left
};

unsigned int indices[6] = {
	0, 1, 2, //Triangle 1
	2, 3, 0  //Triangle 2
};

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//float triangleColor[3] = { 1.0f, 0.5f, 0.0f };

float skyColor[2][3] = {
	// Day Color
	{ 0.5f, 0.0f, 0.5f },
	// Night Color
	{ 0.0f, 0.0f, 0.5f }
};

float sunColor[3] = { 0.75f, 0.0f, 1.0f};
float sunRadius = 0.3f;
float sunSpeed = 1.0f;

float mountainColor[4][3]{
	// Mountain 1
	{ 0.0f, 0.05f, 0.15f }, { 0.0f, 0.0f, 0.0f },
	// Mountain 2
	{ 0.0f, 0.05f, 0.1f }, { 0.0f, 0.0f, 0.0f }
};

//float triangleBrightness = 1.0f;
bool showImGUIDemoWindow = true;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
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

	//Create Shader Program
	dd11::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();

	unsigned int vao = createVAO(vertices, 4, indices, 6);

	glBindVertexArray(vao);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		//shader.setVec3("_Color", triangleColor[0], triangleColor[1], triangleColor[2]);
		//shader.setFloat("_Brightness", triangleBrightness);
		shader.setFloat("iTime", (float)glfwGetTime());
		shader.setVec2("iResolution", (float)SCREEN_HEIGHT, (float)SCREEN_WIDTH);
		shader.setVec3Array("SkyColor", 2, skyColor);
		shader.setVec3Array("MountainColor", 4, mountainColor);
		shader.setVec3("SunColor", sunColor[0], sunColor[1], sunColor[2]);
		shader.setFloat("SunRadius", sunRadius);
		shader.setFloat("SunSpeed", sunSpeed);

		//Wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//Shaded
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("User Interface");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
			ImGui::ColorEdit3("Day Color", skyColor[0]);
			ImGui::ColorEdit3("Night Color", skyColor[1]);
			ImGui::ColorEdit3("Sun Color", sunColor);
			ImGui::SliderFloat("Sun Radius", &sunRadius, 0.0f, 2.0f);
			ImGui::SliderFloat("Sun Speed", &sunSpeed, 0.0f, 10.0f);
			ImGui::ColorEdit3("Higher Mountain Day Color", mountainColor[0]);
			ImGui::ColorEdit3("Higher Mountain Night Color", mountainColor[1]);
			ImGui::ColorEdit3("Lower Mountain Day Color", mountainColor[2]);
			ImGui::ColorEdit3("Lower Mountain Night Color", mountainColor[3]);
			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// EBO
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	// Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 5, vertexData, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, x));
	glEnableVertexAttribArray(0);

	// UV attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, u));
	glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

