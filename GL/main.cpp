#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <utility>
#include <string>
#include <sstream>


class Shader
{
public:
	GLuint ID;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		unsigned int vertex, fragment;
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	void use()
	{
		glUseProgram(ID);
	}
private:
	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n------------------------------------------------------- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n------------------------------------------------------- " << std::endl;
			}
		}
	}
};
class octahedron
{
private:
	std::pair<int, int> line[42];
	int block[42];
	std::queue<std::pair<std::pair<int, int>, int > > taskQ;
	glm::mat4 spinRot[42];
	glm::mat4 temp;
	std::vector< std::vector<int> > face[8];
	std::vector< std::vector<int> > corner[13];
public:
	octahedron();
	glm::mat4 getSpinRot(int i)
	{
		return spinRot[i];
	}
	std::pair<int, int> getPair(int i)
	{
		return line[i];
	}
	void addTask(int i, int j)
	{
		taskQ.push({ { i,j },0 });
	}
	void task();
	void spinFace(int);
	void spinFace2(std::vector<int>);
	void spinCorner(int);
	void spinCorner2(std::vector<int>);
}cube;
octahedron::octahedron()
{
	int i;
	for (i = 0; i < 42; ++i)
	{
		block[i] = i;
		spinRot[i] = glm::mat4(1.0f);
	}
	face[0] = { { 0, 17, 19}, {1, 9, 10}, {5, 18, 6} , {4, 25, 11}, {16, 26, 2} };
	face[1] = { { 0, 19, 21}, {2, 11, 12}, {6, 20, 7} , {1, 27, 13}, {10, 28, 3} };
	face[2] = { { 0, 21, 23}, {3, 13, 14}, {7, 22, 8} , {2, 29, 15}, {12, 30, 4} };
	face[3] = { { 0, 23, 17}, {4, 15, 16}, {8, 24, 5} , {3, 31, 9}, {14, 32, 1} };
	face[4] = { { 41, 19, 17}, {37, 26, 25}, {34, 18, 33} , {38, 10, 32}, {27, 9, 40} };
	face[5] = { { 41, 21, 19}, {38, 28, 27}, {35, 20, 34} , {39, 12, 26}, {29, 11, 37} };
	face[6] = { { 41, 23, 21}, {39, 30, 29}, {36, 22, 35} , {40, 14, 28}, {31, 13, 38} };
	face[7] = { { 41, 17, 23}, {40, 32, 31}, {33, 24, 36} , {37, 16, 30}, {25, 15, 39} };
	corner[0] = { {0} };
	corner[1] = { {1,2,3,4}, {5,6,7,8} ,{0} };
	corner[2] = { {17} };
	corner[3] = { {9,16,32,25}, {18,5,24,33} ,{17} };
	corner[4] = { {19} };
	corner[5] = { {11,10,26,27}, {6,18,34,20} ,{19} };
	corner[6] = { {21} };
	corner[7] = { {13,12,28,29}, {22,7,20,35} ,{21} };
	corner[8] = { {23} };
	corner[9] = { {15,14,30,31}, {8,22,36,24} ,{23} };
	corner[10] = { {41} };
	corner[11] = { {40,39,38,37} ,{36,35,34,33},{41} };

	line[0] = { 0, 12 };
	line[1] = { 12, 3 };
	line[2] = { 15, 3 };
	line[3] = { 18, 3 };
	line[4] = { 21, 3 };
	line[5] = { 24, 6 };
	line[6] = { 30, 6 };
	line[7] = { 36, 6 };
	line[8] = { 42, 6 };
	line[9] = { 48, 3 };
	line[10] = { 51, 3 };
	line[11] = { 54, 3 };
	line[12] = { 57, 3 };
	line[13] = { 60, 3 };
	line[14] = { 63, 3 };
	line[15] = { 66, 3 };
	line[16] = { 69, 3 };
	line[17] = { 72, 12 };
	line[18] = { 84, 6 };
	line[19] = { 90, 12 };
	line[20] = { 102, 6 };
	line[21] = { 108, 12 };
	line[22] = { 120, 6 };
	line[23] = { 126, 12 };
	line[24] = { 138, 6 };
	line[25] = { 144, 3 };
	line[26] = { 147, 3 };
	line[27] = { 150, 3 };
	line[28] = { 153, 3 };
	line[29] = { 156, 3 };
	line[30] = { 159, 3 };
	line[31] = { 162, 3 };
	line[32] = { 165, 3 };
	line[33] = { 168, 6 };
	line[34] = { 174, 6 };
	line[35] = { 180, 6 };
	line[36] = { 186, 6 };
	line[37] = { 192, 3 };
	line[38] = { 195, 3 };
	line[39] = { 198, 3 };
	line[40] = { 201, 3 };
	line[41] = { 204, 12 };
}
void octahedron::task()
{
	int i, q, shift;
	if (!taskQ.empty())
	{
		q = taskQ.front().first.first;
		shift = taskQ.front().first.second;
		if (q == -1 && shift == -1)
		{
			for (i = 0; i < 42; ++i)
			{
				block[i] = i;
				spinRot[i] = glm::mat4(1.0f);
			}
			taskQ.pop();
			return;
		}
		if (taskQ.front().second == 4)
		{
			if (q <= 8)
			{
				if (shift < 0)
					spinFace(q - 1);
				spinFace(q - 1);
			}
			else if (q % 2 == 0)
			{
				if (shift < 0)
				{
					spinCorner(q - 9);
					spinCorner(q - 9);
				}
				spinCorner(q - 9);
			}
			taskQ.pop();
			return;
		}
		switch (q)
		{
		case 1:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6 * shift, glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)));
			for (i = 0; i < 15; ++i)
				spinRot[block[face[0][i / 3][i % 3]]] = temp * spinRot[block[face[0][i / 3][i % 3]]];
			break;
		case 2:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6 * shift, glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)));
			for (i = 0; i < 15; ++i)
				spinRot[block[face[1][i / 3][i % 3]]] = temp * spinRot[block[face[1][i / 3][i % 3]]];
			break;
		case 3:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6 * shift, glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f)));
			for (i = 0; i < 15; ++i)
				spinRot[block[face[2][i / 3][i % 3]]] = temp * spinRot[block[face[2][i / 3][i % 3]]];
			break;
		case 4:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6 * shift, glm::normalize(glm::vec3(-1.0f, 1.0f, 1.0f)));
			for (i = 0; i < 15; ++i)
				spinRot[block[face[3][i / 3][i % 3]]] = temp * spinRot[block[face[3][i / 3][i % 3]]];
			break;
		case 5:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6 * shift, glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f)));
			for (i = 0; i < 15; ++i)
				spinRot[block[face[4][i / 3][i % 3]]] = temp * spinRot[block[face[4][i / 3][i % 3]]];
			break;
		case 6:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6 * shift, glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f)));
			for (i = 0; i < 15; ++i)
				spinRot[block[face[5][i / 3][i % 3]]] = temp * spinRot[block[face[5][i / 3][i % 3]]];
			break;
		case 7:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6 * shift, glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)));
			for (i = 0; i < 15; ++i)
				spinRot[block[face[6][i / 3][i % 3]]] = temp * spinRot[block[face[6][i / 3][i % 3]]];
			break;
		case 8:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 6 * shift, glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f)));
			for (i = 0; i < 15; ++i)
				spinRot[block[face[7][i / 3][i % 3]]] = temp * spinRot[block[face[7][i / 3][i % 3]]];
			break;
		case 9:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
			spinRot[block[corner[0][0][0]]] = temp * spinRot[block[corner[0][0][0]]];
			break;
		case 10:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
			for (i = 0; i < 9; ++i)
				spinRot[block[corner[1][i / 4][i % 4]]] = temp * spinRot[block[corner[1][i / 4][i % 4]]];
			break;
		case 11:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
			spinRot[block[corner[2][0][0]]] = temp * spinRot[block[corner[2][0][0]]];
			break;
		case 12:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
			for (i = 0; i < 9; ++i)
				spinRot[block[corner[3][i / 4][i % 4]]] = temp * spinRot[block[corner[3][i / 4][i % 4]]];
			break;
		case 13:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
			spinRot[block[corner[4][0][0]]] = temp * spinRot[block[corner[4][0][0]]];
			break;
		case 14:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
			for (i = 0; i < 9; ++i)
				spinRot[block[corner[5][i / 4][i % 4]]] = temp * spinRot[block[corner[5][i / 4][i % 4]]];
			break;
		case 15:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)));
			spinRot[block[corner[6][0][0]]] = temp * spinRot[block[corner[6][0][0]]];
			break;
		case 16:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)));
			for (i = 0; i < 9; ++i)
				spinRot[block[corner[7][i / 4][i % 4]]] = temp * spinRot[block[corner[7][i / 4][i % 4]]];
			break;
		case 17:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f)));
			spinRot[block[corner[8][0][0]]] = temp * spinRot[block[corner[8][0][0]]];
			break;
		case 18:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f)));
			for (i = 0; i < 9; ++i)
				spinRot[block[corner[9][i / 4][i % 4]]] = temp * spinRot[block[corner[9][i / 4][i % 4]]];
			break;
		case 19:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)));
			spinRot[block[corner[10][0][0]]] = temp * spinRot[block[corner[10][0][0]]];
			break;
		case 20:
			temp = glm::rotate(glm::mat4(1.0f), glm::pi<float>() / 8 * shift, glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)));
			for (i = 0; i < 9; ++i)
				spinRot[block[corner[11][i / 4][i % 4]]] = temp * spinRot[block[corner[11][i / 4][i % 4]]];
			break;
		}
		++taskQ.front().second;
	}
}
void octahedron::spinFace(int n)
{
	spinFace2(face[n][0]);
	spinFace2(face[n][1]);
	spinFace2(face[n][2]);
	spinFace2(face[n][3]);
	spinFace2(face[n][4]);
}
void octahedron::spinFace2(std::vector<int> a)
{
	int temp;
	temp = block[a[0]];
	block[a[0]] = block[a[1]];
	block[a[1]] = block[a[2]];
	block[a[2]] = temp;
}
void octahedron::spinCorner(int n)
{
	spinCorner2(corner[n][0]);
	spinCorner2(corner[n][1]);
}
void octahedron::spinCorner2(std::vector<int> a)
{
	int temp;
	temp = block[a[0]];
	block[a[0]] = block[a[1]];
	block[a[1]] = block[a[2]];
	block[a[2]] = block[a[3]];
	block[a[3]] = temp;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void init();

GLint shift = 1;
GLboolean drag = false;
GLdouble X = 0, Y = glm::pi<GLdouble>() * 0.5;
GLdouble xpos1, ypos1;
glm::mat4 temp = glm::mat4(1.0f);
glm::vec4 temp2 = glm::vec4(1.0f);
glm::vec4 temp4 = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
glm::mat4 rotx = glm::mat4(1.0f);
glm::mat4 roty = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 rotate = glm::mat4(1.0f);
GLuint rotateLoc;
GLuint VAO;
GLuint VBO[1];
GLfloat vertices[] = {
	0.0f, 0.0f, 3.0f,
	0.0f, 1.0f, 2.0f,
	1.0f, 0.0f, 2.0f, //1-1

	0.0f, 0.0f, 3.0f,
	1.0f, 0.0f, 2.0f,
	0.0f,-1.0f, 2.0f, //1-2

	0.0f, 0.0f, 3.0f,
	0.0f,-1.0f, 2.0f,
	 -1.0f, 0.0f, 2.0f, //1-3

	 0.0f, 0.0f, 3.0f,
	 -1.0f, 0.0f, 2.0f,
	 0.0f, 1.0f, 2.0f, //1-4

	 0.0f, 1.0f, 2.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 0.0f, 2.0f, //2-1

	 0.0f,-1.0f, 2.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f, 0.0f, 2.0f, //3-1

	 0.0f,-1.0f, 2.0f,
	 -1.0f,-1.0f, 1.0f,
	 -1.0f, 0.0f, 2.0f, //4-1

	 0.0f, 1.0f, 2.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f, 0.0f, 2.0f, //5-1

	 0.0f, 1.0f, 2.0f,
	 -1.0f, 1.0f, 1.0f,
	 0.0f, 2.0f, 1.0f, //6-1

	 0.0f, 1.0f, 2.0f,
	 0.0f, 2.0f, 1.0f,
	 1.0f, 1.0f, 1.0f, //6-2

	 1.0f, 0.0f, 2.0f,
	 1.0f, 1.0f, 1.0f,
	 2.0f, 0.0f, 1.0f, //7-1

	 1.0f, 0.0f, 2.0f,
	 2.0f, 0.0f, 1.0f,
	 1.0f,-1.0f, 1.0f, //7-2

	 0.0f,-1.0f, 2.0f,
	 1.0f,-1.0f, 1.0f,
	 0.0f,-2.0f, 1.0f, //8-1

	 0.0f,-1.0f, 2.0f,
	 0.0f,-2.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f, //8-2

	 -1.0f, 0.0f, 2.0f,
	 -1.0f,-1.0f, 1.0f,
	 -2.0f, 0.0f, 1.0f, //9-1

	 -1.0f, 0.0f, 2.0f,
	 -2.0f, 0.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f, //9-2

	 0.0f, 2.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	 1.0f, 2.0f, 0.0f, //10-1

	 1.0f, 1.0f, 1.0f,
	 2.0f, 0.0f, 1.0f,
	 2.0f, 1.0f, 0.0f, //11-1

	 1.0f,-1.0f, 1.0f,
	 2.0f, 0.0f, 1.0f,
	 2.0f,-1.0f, 0.0f, //12-1

	 0.0f,-2.0f, 1.0f,
	 1.0f,-1.0f, 1.0f,
	 1.0f,-2.0f, 0.0f, //13-1

	 0.0f,-2.0f, 1.0f,
	 -1.0f,-1.0f, 1.0f,
	 -1.0f,-2.0f, 0.0f, //14-1

	 -1.0f,-1.0f, 1.0f,
	 -2.0f, 0.0f, 1.0f,
	 -2.0f,-1.0f, 0.0f, //15-1

	 -1.0f, 1.0f, 1.0f,
	 -2.0f, 0.0f, 1.0f,
	 -2.0f, 1.0f, 0.0f, //16-1

	 0.0f, 2.0f, 1.0f,
	 -1.0f, 1.0f, 1.0f,
	 -1.0f, 2.0f, 0.0f, //17-1

	 0.0f, 3.0f, 0.0f,
	 0.0f, 2.0f, 1.0f,
	 1.0f, 2.0f, 0.0f, //18-1

	 0.0f, 3.0f, 0.0f,
	 1.0f, 2.0f, 0.0f,
	 0.0f, 2.0f,-1.0f, //18-2

	 0.0f, 3.0f, 0.0f,
	 0.0f, 2.0f,-1.0f,
	 -1.0f, 2.0f, 0.0f, //18-3

	 0.0f, 3.0f, 0.0f,
	 -1.0f, 2.0f, 0.0f,
	 0.0f, 2.0f, 1.0f, //18-4

	 1.0f, 1.0f, 1.0f,
	 1.0f, 2.0f, 0.0f,
	 2.0f, 1.0f, 0.0f, //19-1

	 2.0f, 1.0f, 0.0f,
	 1.0f, 2.0f, 0.0f,
	 1.0f, 1.0f,-1.0f, //19-2

	 3.0f, 0.0f, 0.0f,
	 2.0f, 0.0f, 1.0f,
	 2.0f, 1.0f, 0.0f, //20-1

	 3.0f, 0.0f, 0.0f,
	 2.0f, 1.0f, 0.0f,
	 2.0f, 0.0f,-1.0f, //20-2

	 3.0f, 0.0f, 0.0f,
	 2.0f, 0.0f,-1.0f,
	 2.0f,-1.0f, 0.0f, //20-3

	 3.0f, 0.0f, 0.0f,
	 2.0f,-1.0f, 0.0f,
	 2.0f, 0.0f, 1.0f, //20-4

	 1.0f,-1.0f, 1.0f,
	 1.0f,-2.0f, 0.0f,
	 2.0f,-1.0f, 0.0f, //21-1

	 2.0f,-1.0f, 0.0f,
	 1.0f,-2.0f, 0.0f,
	 1.0f,-1.0f,-1.0f, //21-2

	 0.0f,-3.0f, 0.0f,
	 0.0f,-2.0f,-1.0f,
	-1.0f,-2.0f, 0.0f, //22-1

	 0.0f,-3.0f, 0.0f,
	-1.0f,-2.0f, 0.0f,
	 0.0f,-2.0f, 1.0f, //22-2

	 0.0f,-3.0f, 0.0f,
	 0.0f,-2.0f, 1.0f,
	 1.0f,-2.0f, 0.0f, //22-3

	 0.0f,-3.0f, 0.0f,
	 1.0f,-2.0f, 0.0f,
	 0.0f,-2.0f,-1.0f, //22-4

	-1.0f,-1.0f,-1.0f,
	-1.0f,-2.0f, 0.0f,
	-2.0f,-1.0f, 0.0f, //23-1

	-2.0f,-1.0f, 0.0f,
	-1.0f,-2.0f, 0.0f,
	-1.0f,-1.0f, 1.0f, //23-2

	-3.0f, 0.0f, 0.0f,
	-2.0f, 0.0f,-1.0f,
	-2.0f,-1.0f, 0.0f, //24-1

	-3.0f, 0.0f, 0.0f,
	-2.0f,-1.0f, 0.0f,
	-2.0f, 0.0f, 1.0f, //24-2

	-3.0f, 0.0f, 0.0f,
	-2.0f, 0.0f, 1.0f,
	-2.0f, 1.0f, 0.0f, //24-3

	-3.0f, 0.0f, 0.0f,
	-2.0f, 1.0f, 0.0f,
	-2.0f, 0.0f,-1.0f, //24-4

	-1.0f, 1.0f,-1.0f,
	-1.0f, 2.0f, 0.0f,
	-2.0f, 1.0f, 0.0f, //25-1

	-2.0f, 1.0f, 0.0f,
	-1.0f, 2.0f, 0.0f,
	-1.0f, 1.0f, 1.0f, //25-2

	 0.0f, 2.0f,-1.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f, 2.0f, 0.0f, //26-1

	 1.0f, 1.0f,-1.0f,
	 2.0f, 0.0f,-1.0f,
	 2.0f, 1.0f, 0.0f, //27-1

	 1.0f,-1.0f,-1.0f,
	 2.0f, 0.0f,-1.0f,
	 2.0f,-1.0f, 0.0f, //28-1

	 0.0f,-2.0f,-1.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f,-2.0f, 0.0f, //29-1

	 0.0f,-2.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-2.0f, 0.0f, //30-1

	-1.0f,-1.0f,-1.0f,
	-2.0f, 0.0f,-1.0f,
	-2.0f,-1.0f, 0.0f, //31-1

	-1.0f, 1.0f,-1.0f,
	-2.0f, 0.0f,-1.0f,
	-2.0f, 1.0f, 0.0f, //32-1

	 0.0f, 2.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	 -1.0f, 2.0f, 0.0f, //33-1

	 0.0f, 1.0f,-2.0f,
	 -1.0f, 1.0f,-1.0f,
	 0.0f, 2.0f,-1.0f, //34-1

	 0.0f, 1.0f,-2.0f,
	 0.0f, 2.0f,-1.0f,
	 1.0f, 1.0f,-1.0f, //34-2

	 1.0f, 0.0f,-2.0f,
	 1.0f, 1.0f,-1.0f,
	 2.0f, 0.0f,-1.0f, //35-1

	 1.0f, 0.0f,-2.0f,
	 2.0f, 0.0f,-1.0f,
	 1.0f,-1.0f,-1.0f, //35-2

	 0.0f,-1.0f,-2.0f,
	 1.0f,-1.0f,-1.0f,
	 0.0f,-2.0f,-1.0f, //36-1

	 0.0f,-1.0f,-2.0f,
	 0.0f,-2.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f, //36-2	 

	 -1.0f, 0.0f,-2.0f,
	 -1.0f,-1.0f,-1.0f,
	 -2.0f, 0.0f,-1.0f, //37-1

	 -1.0f, 0.0f,-2.0f,
	 -2.0f, 0.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f, //37-2

	 0.0f, 1.0f,-2.0f,
	 1.0f, 1.0f,-1.0f,
	 1.0f, 0.0f,-2.0f, //38-1

	 0.0f,-1.0f,-2.0f,
	 1.0f,-1.0f,-1.0f,
	 1.0f, 0.0f,-2.0f, //39-1

	 0.0f,-1.0f,-2.0f,
	 -1.0f,-1.0f,-1.0f,
	 -1.0f, 0.0f,-2.0f, //40-1

	 0.0f, 1.0f,-2.0f,
	 -1.0f, 1.0f,-1.0f,
	 -1.0f, 0.0f,-2.0f, //41-1

	 0.0f, 0.0f,-3.0f,
	 0.0f,-1.0f,-2.0f,
	 -1.0f, 0.0f,-2.0f, //42-1

	 0.0f, 0.0f,-3.0f,
	 -1.0f, 0.0f,-2.0f,
	 0.0f, 1.0f,-2.0f, //42-2

	 0.0f, 0.0f,-3.0f,
	 0.0f, 1.0f,-2.0f,
	 1.0f, 0.0f,-2.0f, //42-3

	 0.0f, 0.0f,-3.0f,
	 1.0f, 0.0f,-2.0f,
	 0.0f,-1.0f,-2.0f, //42-4
};
GLfloat colors[] = {
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,//1-1

	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,//1-2

	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,//1-3

	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,//1-4

	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,//2-1

	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,//3-1

	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,//4-1

	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,//5-1

	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,//6-1

	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,//6-2

	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,//7-1

	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,//7-2

	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,//8-1

	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,//8-2

	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,//9-1

	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,//9-2

	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,//10-1

	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,//11-1

	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,//12-1

	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,//13-1

	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,//14-1

	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,//15-1

	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,//16-1

	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,//17-1

	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,//18-1

	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,//18-2

	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,//18-3

	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,//18-4

	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,//19-1

	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,//19-2

	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,
	0.25f,0.25f,0.25f,//20-1

	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,//20-2

	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,//20-3

	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,//20-4

	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,//21-1

	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,//21-2

	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,//22-1

	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,//22-2

	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,
	0.9f,0.0f,0.0f,//22-3

	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,//22-4

	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,//23-1

	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,//23-2

	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,//24-1

	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,
	0.0f,0.9f,0.0f,//24-2

	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,//24-3

	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,//24-4

	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,//25-1

	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,
	0.0f,0.0f,0.9f,//25-2

	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,//26-1

	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,//27-1

	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,//28-1

	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,//29-1

	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,//30-1

	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,//31-1

	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,//32-1

	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,//33-1

	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,//34-1

	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,//34-2

	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,//35-1

	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,//35-2

	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,//36-1

	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,//36-2

	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,//37-1

	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,//37-2

	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,//38-1

	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,//39-1

	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,//40-1

	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,//41-1

	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,
	0.9f,0.9f,0.0f,//24-1

	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,
	0.0f,0.9f,0.9f,//24-2

	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,
	0.9f,0.0f,0.9f,//24-3

	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f,
	0.9f,0.9f,0.9f//24-4
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		shift = -1;
	else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		shift = 1;
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		cube.addTask(1, shift);
	else if (key == GLFW_KEY_E && action == GLFW_PRESS)
		cube.addTask(2, shift);
	else if (key == GLFW_KEY_C && action == GLFW_PRESS)
		cube.addTask(3, shift);
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		cube.addTask(4, shift);
	else if (key == GLFW_KEY_W && action == GLFW_PRESS)
		cube.addTask(5, shift);
	else if (key == GLFW_KEY_D && action == GLFW_PRESS)
		cube.addTask(6, shift);
	else if (key == GLFW_KEY_X && action == GLFW_PRESS)
		cube.addTask(7, shift);
	else if (key == GLFW_KEY_A && action == GLFW_PRESS)
		cube.addTask(8, shift);
	else if (key == GLFW_KEY_T && action == GLFW_PRESS)
		cube.addTask(9, shift);
	else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
		cube.addTask(10, shift);
	else if (key == GLFW_KEY_G && action == GLFW_PRESS)
		cube.addTask(11, shift);
	else if (key == GLFW_KEY_H && action == GLFW_PRESS)
		cube.addTask(12, shift);
	else if (key == GLFW_KEY_B && action == GLFW_PRESS)
		cube.addTask(13, shift);
	else if (key == GLFW_KEY_N && action == GLFW_PRESS)
		cube.addTask(14, shift);
	else if (key == GLFW_KEY_I && action == GLFW_PRESS)
		cube.addTask(15, shift);
	else if (key == GLFW_KEY_U && action == GLFW_PRESS)
		cube.addTask(16, shift);
	else if (key == GLFW_KEY_K && action == GLFW_PRESS)
		cube.addTask(17, shift);
	else if (key == GLFW_KEY_J && action == GLFW_PRESS)
		cube.addTask(18, shift);
	else if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
		cube.addTask(19, shift);
	else if (key == GLFW_KEY_M && action == GLFW_PRESS)
		cube.addTask(20, shift);
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
		cube.addTask(-1, -1);
	//	else if (key == GLFW_KEY_F && action == GLFW_PRESS)
	//		cube.addTask(-1, 0);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		drag = true;
		glfwGetCursorPos(window, &xpos1, &ypos1);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		drag = false;
}
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (drag)
	{
		X -= (xpos - xpos1) / 128;
		if (Y - (ypos - ypos1) / 128 > 0 && Y - (ypos - ypos1) / 128 < glm::pi<GLdouble>())
			Y -= (ypos - ypos1) / 128;
		float camX = sin(X) * sin(Y) * 10;
		float camY = cos(Y) * 10;
		float camZ = cos(X) * sin(Y) * 10;
		view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		xpos1 = xpos;
		ypos1 = ypos;
	}
}

void init()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors) + sizeof(vertices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)sizeof(vertices));
	glEnableVertexAttribArray(1);

}

int main()
{
	int i;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1000, 1000, "testOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return-1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return-1;
	}
	Shader cubeShader("glCube.vs", "glCube.fs");
	init();
	cubeShader.use();
	GLuint modelLoc;
	GLuint transformlLoc;
	GLuint viewLoc;
	GLuint spinLoc;
	GLuint projectionLoc;
	std::pair<int, int> tempPair;
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	view = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	projection = glm::perspective(glm::radians(45.0f), 1000.f / 1000, 0.1f, 100.0f);
	 
	cubeShader.use();

	rotateLoc = glGetUniformLocation(cubeShader.ID, "rotate");
	modelLoc = glGetUniformLocation(cubeShader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	spinLoc = glGetUniformLocation(cubeShader.ID, "spin");
	viewLoc = glGetUniformLocation(cubeShader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	projectionLoc = glGetUniformLocation(cubeShader.ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		cube.task();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (i = 0; i < 42; ++i)
		{
			glUniformMatrix4fv(spinLoc, 1, GL_FALSE, glm::value_ptr(cube.getSpinRot(i)));
			tempPair = cube.getPair(i);
			glDrawArrays(GL_TRIANGLES, tempPair.first, tempPair.second);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, VBO);
	glfwTerminate();
	return 0;
}
