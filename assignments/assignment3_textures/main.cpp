	#include <stdio.h>
	#include <math.h>

	#include <ew/external/glad.h>
	#include <ew/ewMath/ewMath.h>
	#include <GLFW/glfw3.h>
	#include <imgui.h>
	#include <imgui_impl_glfw.h>
	#include <imgui_impl_opengl3.h>

	#include <ew/shader.h>
	#include <dd11/texture.h>

	struct Vertex {
		float x, y, z;
		float u, v;
	};

	unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices);
	void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	const int SCREEN_WIDTH = 1080;
	const int SCREEN_HEIGHT = 720;

	Vertex vertices[4] = {
		{-1.0, -1.0, 0.0, 0.0, 0.0},
		{1.0, -1.0, 0.0, 1.0, 0.0},
		{1.0, 1.0, 0.0, 1.0, 1.0},
		{-1.0, 1.0, 0.0, 0.0, 1.0}
	};
	unsigned short indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	float distStrength = 0.5f, moveSpeed = 1.0f, tiling = 1.0f, charSize = 1.0f;

	int main() {
		printf("Initializing...");
		if (!glfwInit()) {
			printf("GLFW failed to init!");
			return 1;
		}

		GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures", NULL, NULL);
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

		ew::Shader backgroundShader("assets/background.vert", "assets/background.frag");
		ew::Shader characterShader("assets/character.vert", "assets/character.frag");

		unsigned int quadVAO = createVAO(vertices, 4, indices, 6);

		unsigned int textureA = loadTexture("assets/sky.jpg", GL_REPEAT, GL_LINEAR);
		unsigned int textureB = loadTexture("assets/sun.png", GL_REPEAT, GL_LINEAR);

		unsigned int distortion = loadTexture("assets/noiseTexture.png", GL_REPEAT, GL_LINEAR);

		unsigned int character = loadTexture("assets/character.png", GL_REPEAT, GL_NEAREST);

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			//Bind Quad
			glBindVertexArray(quadVAO);

			//Background Shader
			backgroundShader.use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureA);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textureB);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, distortion);

			//Uniforms
			backgroundShader.setInt("_SkyTexture", 0);
			backgroundShader.setInt("_SunTexture", 1);
			backgroundShader.setInt("_DistortTexture", 2);
			backgroundShader.setFloat("iTime", (float)glfwGetTime());
			backgroundShader.setFloat("_DistStrength", distStrength);
			backgroundShader.setFloat("_Tiling", tiling);
		
			//Draw Quad
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

			//Character Shader
			characterShader.use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, character);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			//Uniforms
			characterShader.setInt("_CharacterShader", 0);
			characterShader.setFloat("iTime", (float)glfwGetTime());
			characterShader.setFloat("_moveSpeed", moveSpeed);
			characterShader.setFloat("_charSize", charSize);
		
			//Draw Quad
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

			//Render UI
			{
				ImGui_ImplGlfw_NewFrame();
				ImGui_ImplOpenGL3_NewFrame();
				ImGui::NewFrame();

				ImGui::Begin("Settings");
				ImGui::SliderFloat("Distortion Strength", &distStrength, 0.0f, 5.0f);
				ImGui::SliderFloat("Character Move Speed", &moveSpeed, 0.0f, 10.0f);
				ImGui::SliderFloat("Character Size", &charSize, 0.3f, 5.0f);
				ImGui::SliderFloat("Tiling", &tiling, 1.0f, 10.0f);
				ImGui::End();

				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}

			glfwSwapBuffers(window);
		}
		printf("Shutting down...");
	}

	unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices) {
		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//Vertex Buffer Object 
		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertexData, GL_STATIC_DRAW);

		//Element Buffer Object
		unsigned int ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * numIndices, indicesData, GL_STATIC_DRAW);

		//Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex,x));
		glEnableVertexAttribArray(0);

		//UV attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, u)));
		glEnableVertexAttribArray(1);

		return vao;
	}

	void framebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

