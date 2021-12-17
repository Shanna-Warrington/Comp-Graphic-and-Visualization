
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "camera.h"
#include "cylinder.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Math libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

//Window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Lighting
glm::vec3 lightPos(0.0f, 5.0f, 0.0f);
glm::vec3 lightPos2(6.0f, 0.05f, 0.0f);

float xlight = 0.0f, ylight = 5.0f, zlight = 0.0f;

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool orthographic = false;

//Timing
float deltaTime = 0.0f;	//Time between the current frame and the last frame
float lastFrame = 0.0f;



int main() {
	
	//instantiates the GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//Creating a window.
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shanna's Project", NULL, NULL);
	if (window == NULL) {

		std::cout << "Failure to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//Tells GLFW to capture the mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Glad manages function pointers for OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failure to initialize GLAD" << std::endl;
		return -1;
	};

	//Configures global opengl state
	glEnable(GL_DEPTH_TEST);

	//Building and compiling our shader program
	Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");
	Shader lightCubeShader("shaderfiles/2.2.light_cube.vs", "shaderfiles/2.2.light_cube.fs");



	//Configure light cube VAO and VBO
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	//Vertices for the shape
	float vertices[] = {

		//CUBE
		//Position			  //Texture coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

		//PLANE
		//Positions            //Texture coords
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,   0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,   0.0f, 1.0f,

		//Pyramid
		//Positions             //Texture coords
		-0.5f,  -0.5f, -0.5f,   0.0f, 0.0f,
		 0.5f,  -0.5f, -0.5f,   1.0f, 0.0f,
		 0.0f,   0.5f,  0.0f,   0.5f, 1.0f,

		-0.5f,  -0.5f,  0.5f,   0.0f, 0.0f,
		 0.5f,  -0.5f,  0.5f,   1.0f, 0.0f,
		 0.0f,   0.5f,  0.0f,   0.5f, 1.0f,

		-0.5f,  -0.5f, -0.5f,   0.0f, 1.0f,
		-0.5f,  -0.5f,  0.5f,   0.0f, 0.0f,
		 0.0f,   0.5f,  0.0f,   0.5f, 1.0f,

		 0.5f,  -0.5f, -0.5f,   0.0f, 1.0f,
		 0.5f,  -0.5f,  0.5f,   0.0f, 0.0f,
		 0.0f,   0.5f,  0.0f,   0.5f, 1.0f,

		 0.5f,  -0.5f, -0.5f,   1.0f, 1.0f,
		 0.5f,  -0.5f,  0.5f,   1.0f, 0.0f,
		 0.0f,   0.5f,  0.0f,   0.5f, 1.0f,

		-0.5f,  -0.5f,  0.5f,   0.0f, 0.0f,
		-0.5f,  -0.5f, -0.5f,   0.0f, 1.0f,
		 0.0f,   0.5f,  0.0f,   0.5f, 1.0f,

	};


	//Generating a Vertex Buffer Object and Vertex Attribut object, then binding it
	unsigned int VBO, VAO;
	unsigned int cubeVBO, cubeVAO;
	unsigned int cylinderVBO, cylinderVAO;
	unsigned int cylinderVBO2, cylinderVAO2;
	unsigned int bookVBO, bookVAO;
	unsigned int pyrVBO, pyrVAO;

	//Plane------------------------------------------
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//Bind the vertex array
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Calls the buffer data(type of buffer to copy data into, size of data, actual data we want to send, how the graphics card uses the data)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Texture Coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//--------------------------------------------------
	//Cube object(bottle)
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Texture Coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//-----------------------------------------------
	//Cube object(book)
	glGenVertexArrays(1, &bookVAO);
	glGenBuffers(1, &bookVBO);
	glBindVertexArray(bookVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bookVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Texture Coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//------------------------------------------------
	//cylinder object(bottle cap)
	glGenVertexArrays(1, &cylinderVAO);
	glGenBuffers(1, &cylinderVBO);
	glBindVertexArray(cylinderVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);

	//------------------------------------------------
	//cylinder object(speaker)
	glGenVertexArrays(1, &cylinderVAO2);
	glGenBuffers(1, &cylinderVBO2);
	glBindVertexArray(cylinderVAO2);
	glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO2);

	//--------------------------------------------------
	//Pyramid container
	glGenVertexArrays(1, &pyrVAO);
	glGenBuffers(1, &pyrVBO);
	glBindVertexArray(pyrVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pyrVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Position and texture
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//-----------------------------------------------------

	//Load and create texture
	unsigned int texture, texture2, texture3, texture4, texture5, texture6, texture7;

	//First texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Sets the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Sets the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Loads the image, create texture, and generates mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); //flips the texture
	unsigned char* data = stbi_load("Background.jpg", &width, &height, &nrChannels, 0);
	if (data) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failure to load texture" << std::endl;
	}
	stbi_image_free(data);
	
	//second texture
	//Load and create texture
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	//Sets the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Sets the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Loads the image, create texture, and generates mipmaps
	unsigned char* data2 = stbi_load("polish-bottle.jpg", &width, &height, &nrChannels, 0);
	if (data) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failure to load texture" << std::endl;
	}
	stbi_image_free(data2);

	//Third texture
	//Load and create texture
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);

	//Sets the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Sets the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Loads the image, create texture, and generates mipmaps
	unsigned char* data3 = stbi_load("bottle-cap.jpg", &width, &height, &nrChannels, 0);
	if (data) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failure to load texture" << std::endl;
	}
	stbi_image_free(data3);

	//Fourth texture
	//Load and create texture
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);

	//Sets the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Sets the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Loads the image, create texture, and generates mipmaps
	unsigned char* data4 = stbi_load("speaker.jpg", &width, &height, &nrChannels, 0);
	if (data) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failure to load texture" << std::endl;
	}
	stbi_image_free(data4);

	//Fifth texture
	//Load and create texture
	glGenTextures(1, &texture5);
	glBindTexture(GL_TEXTURE_2D, texture5);

	//Sets the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Sets the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Loads the image, create texture, and generates mipmaps
	unsigned char* data5 = stbi_load("brown-leather.jpg", &width, &height, &nrChannels, 0);
	if (data) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failure to load texture" << std::endl;
	}
	stbi_image_free(data5);

	//Sixth texture
	//Load and create texture
	glGenTextures(1, &texture6);
	glBindTexture(GL_TEXTURE_2D, texture6);

	//Sets the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Sets the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Loads the image, create texture, and generates mipmaps
	unsigned char* data6 = stbi_load("book.jpg", &width, &height, &nrChannels, 0);
	if (data) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data6);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failure to load texture" << std::endl;
	}
	stbi_image_free(data6);

	//Seventh texture
	//Load and create texture
	glGenTextures(1, &texture7);
	glBindTexture(GL_TEXTURE_2D, texture7);

	//Sets the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Sets the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Loads the image, create texture, and generates mipmaps
	unsigned char* data7 = stbi_load("red-checker.jpg", &width, &height, &nrChannels, 0);
	if (data) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data7);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failure to load texture" << std::endl;
	}
	stbi_image_free(data7);

	ourShader.use();
	ourShader.setInt("texture", 0);
	ourShader.setInt("texture2", 1);
	ourShader.setInt("texture3", 2);
	ourShader.setInt("texture4", 3);
	ourShader.setInt("texture5", 4);
	ourShader.setInt("texture6", 5);
	ourShader.setInt("texture7", 6);
	

	glm::mat4 model;
	float angle;

	//Render loop: will keep running until told to stop
	while (!glfwWindowShouldClose(window)) {

		//Per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Input
		processInput(window);

		lightPos[0] = xlight;
		lightPos[1] = ylight;
		lightPos[2] = zlight;


		//Render commands go here
		
		//Clears the frame
		glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		

		//Activates shader
		ourShader.use();

		//Passes the projection matrix to shader
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		//Ortho perspective
		/*float scale = 100;
		glm::mat4 projection = glm::ortho(-((float)SCR_WIDTH / scale), (float)SCR_WIDTH / scale, (float)SCR_HEIGHT / scale, -((float)SCR_HEIGHT / scale),  0.1f, 100.0f);*/

		//Changes the view from perspective to ortho and back
		/*glm::mat4 projection;
		if (orthographic) {

			float scale = 100;
			glm::mat4 projection = glm::ortho(-((float)SCR_WIDTH / scale), (float)SCR_WIDTH / scale, (float)SCR_HEIGHT / scale, -((float)SCR_HEIGHT / scale),  0.1f, 100.0f);

		}
		else {

			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		}*/

		ourShader.setMat4("projection", projection);


		//Camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		//CUBE---------------------------------------
		//Renders the shape
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(cubeVAO);
		//Creates transformations
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.4, 0.5, 0.3));
		model = glm::translate(model, glm::vec3(3.0f, -4.5f, 0.0f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);
		
		//Draws the shape
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//--------------------------------------------

		//book---------------------------------------
		//Renders the shape
		glActiveTexture(GL_TEXTURE0);			//Base texture of brown leather
		glBindTexture(GL_TEXTURE_2D, texture5);
		//glActiveTexture(GL_TEXTURE1);			//overlap texture of the book title.
		//glBindTexture(GL_TEXTURE_2D, texture6);
		glBindVertexArray(bookVAO);
		//Creates transformations
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5, 0.5, 2.0));
		model = glm::translate(model, glm::vec3(-0.05f, -4.5f, 1.0f));
		ourShader.setMat4("model", model);

		//Draws the shape
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//--------------------------------------------

		//PLANE---------------------------------------
		//Renders the shape
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		//Creates transformations
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(7.0, 5.0, 7.0));
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);

		//Draws the shape
		glDrawArrays(GL_TRIANGLES, 36, 6);
		//-----------------------------------------------
		
		//CYLINDER---------------------------------------
		//Renders the shape
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glBindVertexArray(VAO);
		//Creates transformations
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.25, 0.5, 0.25));
		model = glm::translate(model, glm::vec3(4.75f, -4.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C(0.25, 20, 1, true, true, true);
		C.render();

		//-------------------------------------------------

		//CYLINDER2---------------------------------------
		//Renders the shape
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture4);
		glBindVertexArray(VAO);
		//Creates transformations
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.4, 1.25, 0.4));
		model = glm::translate(model, glm::vec3(0.0f, -1.5f, -1.5f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C2(2, 20, 1, true, true, true);
		C2.render();
		//----------------------------------------------------

		//Pyramid container
		//Renders the shape
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture7);
		glBindVertexArray(pyrVAO);
		//Creates transformations
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(2.5, 2.5, 1.0));
		model = glm::translate(model, glm::vec3(-0.5f, -0.5f, -2.0f));
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		ourShader.setMat4("model", model);

		//Draws the shape
		glDrawArrays(GL_TRIANGLES, 42, 18);
		//----------------------------------------------------------

		//Light cube
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); 
		lightCubeShader.setMat4("model", model);

		static_meshes_3D::Cylinder C3(1, 30, 1.5, true, true, true);
		C3.render();

		glBindVertexArray(lightCubeVAO);

		//Second light source
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos2);
		model = glm::scale(model, glm::vec3(0.2f));
		lightCubeShader.setMat4("model", model);

		static_meshes_3D::Cylinder C4(1, 30, 1.5, true, true, true);
		C4.render();

		glBindVertexArray(lightCubeVAO);

		//Swaps buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//De-allocates resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	glDeleteVertexArrays(1, &cylinderVAO);
	glDeleteBuffers(1, &cylinderVBO);

	glDeleteVertexArrays(1, &cylinderVAO2);
	glDeleteBuffers(1, &cylinderVBO2);

	glDeleteVertexArrays(1, &bookVAO);
	glDeleteBuffers(1, &bookVBO);
	
	glDeleteVertexArrays(1, &pyrVAO);
	glDeleteBuffers(1, &pyrVBO);

	//Cleans up the glfw resources
	glfwTerminate();
	return 0;

}

void processInput(GLFWwindow *window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		ylight += 0.001;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		ylight -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		xlight += 0.001;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		xlight -= 0.001;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		zlight += 0.001;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		zlight -= 0.001;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		orthographic = !orthographic;
}

//function called whenever the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	//Tells OpenGL the size of the rendering window
	glViewport(0, 0, width, height);
}

//GLFW: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; //Reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

//GLFW: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}