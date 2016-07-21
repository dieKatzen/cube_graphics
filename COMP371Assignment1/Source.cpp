
#include <cmath>

// GLEW
# define GLEW_STATIC
# include <GL/glew.h>
// GLFW
# include <GLFW/glfw3.h>
# include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

glm::mat4 projection, view, model1, model2;
GLfloat angle = 0.0f;

// Shaders
/*
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";
*/

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform vec4 myColor;\n"
"out vec4 outColor;\n"
//"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(position, 1.0);\n"
"outColor = myColor;"
""
"}\0";



const GLchar* fragmentShaderSource = "#version 330 core\n"
"uniform mat4 view;\n"
"in vec4 outColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = outColor;\n"
"}\n\0";

//
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

float ang = 0.001f;
float zoom = 10;
float up = 10;
float camDis = 5;
bool lines = false;
bool fill = true;
bool points = false;
float redB = 1.0;


int main() {

	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment1", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to created GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	glewExperimental = GL_TRUE;

	glewInit();
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialized GLEW" << std::endl;
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);


	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLfloat vertices[] = {
		
		0.5f,  0.5f, 0.5f,  //0
		0.5f, -0.5f, 0.5f,  //1
		-0.5f,  0.5f, 0.5f, //2
		-0.5f, -0.5f, 0.5f, //3

		0.5f,  0.5f, -0.5f, //4
		0.5f, -0.5f, -0.5f, //5
		-0.5f,  0.5f, -0.5f,//6
		-0.5f, -0.5f, -0.5f,//7
	};

	GLuint indices[] = {  
		0, 1, 2,				//front
		2, 1, 3,

		0, 4, 5,				//right
		1, 0, 5,

		4, 5, 7,				//back
		4, 7, 6,

		2, 3, 6,				//left
		3, 6, 7,

		0, 2, 4,				//top
		2, 4, 6,

		1, 5, 7,				//bottom
		1, 7, 3,
	};
	


	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO


	projection = glm::perspective(45.0f, 1.0f, 0.1f, 20.0f);
	view = glm::lookAt(glm::vec3(0, 10, zoom), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	//model2 = glm::translate(model2, glm::vec3(3, 0, 4));
	//model2 = glm::mat4();

	// Uncommenting this call will result in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

						  // Game loop
						  // Clear the colorbuffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		// Render
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		// Draw our first triangle
		glUseProgram(shaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &(projection)[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &(view)[0][0]);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &(model1)[0][0]); //blue
		glUniform4fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &(glm::vec4(0, 0, 1, 1))[0]);

		//glDrawArrays(GL_TRIANGLES, 0, 6);

		if (!points) {
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		if (points) {
		glDrawElements(GL_POINTS, 36, GL_UNSIGNED_INT,0);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(5.0);
		}
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &(model2)[0][0]); //red
		glUniform4fv(glGetUniformLocation(shaderProgram, "myColor"), 1, &(glm::vec4(1, 0, 0, 1))[0]);

		if (!points) {
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		}

		if (points) {
			glDrawElements(GL_POINTS, 36, GL_UNSIGNED_INT, 0);
			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize(5.0);
		}

		glBindVertexArray(0);

		if(lines){ glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
		if (fill) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
		


		// Swap the screen buffers
		glfwSwapBuffers(window);
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		model1 = glm::rotate(model1, 0.001f, glm::vec3(0, 1, 0));

		glm::mat3 conv;
		ang -= 0.001f;

		conv[0] = glm::vec3(cos(ang), 0 , sin(ang));
		conv[1] = glm::vec3(0,1,0);
		conv[2] = glm::vec3(-sin(ang), 0, cos(ang));

		glm::vec3 rotVec = glm::vec3(3, 0, 4);
		rotVec = conv * rotVec;
		//model2 = glm::translate(glm::scale(glm::mat4(), model2[0][0] * glm::vec3(1,1,1)), rotVec);
		model2 = glm::scale(glm::translate(glm::mat4() , rotVec), glm::vec3(redB, redB, redB));
		//model2 = glm::translate(model2);

		view = glm::lookAt(glm::vec3(0, zoom, 10), glm::vec3(0, 0, 0), glm::vec3(0, up, 0));
		//model2 = glm::scale(glm::translate(glm::mat4(), glm::vec3(-sin(ang), 0, cos(ang))),glm::vec3(redB,redB,redB));
		//model2 = glm::rotate(model2, 0.001f, glm::vec3(0, 1, 0));


		//model1 = glm::rotate(model1, 0.001f, glm::vec3(0, 1, 0));
		//model2 = glm::translate(model2, rotVec);
		//model2 = glm::rotate(model2, 0.001f, glm::vec3(0, 1, 0));
		//model2 = glm::translate(model2, glm::vec3(3, 0, 4));
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFWvoid 
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_P && action != GLFW_RELEASE) {
		model1 = glm::scale(model1, glm::vec3(0.9f, 0.9f, 0.9f));
	}

	if (key == GLFW_KEY_O && action != GLFW_RELEASE) {
		model1 = glm::scale(model1, glm::vec3(1.1f, 1.1f, 1.1f));
	}

	if (key == GLFW_KEY_L && action != GLFW_RELEASE) {
		model2 = glm::scale(model2, glm::vec3(0.9f, 0.9f, 0.9f));
		redB /= 1.1;
	}

	if (key == GLFW_KEY_K && action != GLFW_RELEASE) {
		model2 = glm::scale(model2, glm::vec3(1.1f, 1.1f, 1.1f));
		redB *= 1.1;
	}

	if (key == GLFW_KEY_Z && action != GLFW_RELEASE) {
		zoom += 1;
	}

	if (key == GLFW_KEY_X && action != GLFW_RELEASE) {
		zoom -= 1;
	}

	if (key == GLFW_KEY_C && action != GLFW_RELEASE) {
		up += 1;
	}

	if (key == GLFW_KEY_V && action != GLFW_RELEASE) {
		up -= 1;
	}

	if (key == GLFW_KEY_T && action != GLFW_RELEASE) {
		fill = true;
		lines = false;
		points = false;
	}

	if (key == GLFW_KEY_Y && action != GLFW_RELEASE) {
		fill = false;
		lines = true;
		points = false;
	}

	if (key == GLFW_KEY_U && action != GLFW_RELEASE) {
		fill = false;
		lines = false;
		points = true;
	}

}


