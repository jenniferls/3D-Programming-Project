//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//     - updated by FLL
//--------------------------------------------------------------------------------------
#include <windows.h>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

#define IMGUI_IMPL_OPENGL_LOADER_GLEW

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glew/include/GL/glew.h"
#include "glfw/include/GLFW/glfw3.h"

// read docs: https://github.com/g-truc/glm/blob/master/manual.md#section1
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"
#include <gl/GL.h>

#include "stb_image.h"

#include "GameTimer.h"
#include "RawModel.h"
#include "OBJLoader.h"
#include "Scene.h"
#include "Light.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

#define _360_DEGREES 6.2831853072f //In radians
#define VERTEX_SIZE sizeof(RawModel::TriangleVertex)
#define ANIM_VERTEX_SIZE sizeof(AnimatedModel::Vertex)
#define VERTEX_JOINT_DATA_SIZE sizeof(AnimatedModel::VertexJointData)

#define WIDTH 1280.0f
#define HEIGHT 720.0f
GLFWwindow *gWindow;
using namespace std;

void initWindow(unsigned int w, unsigned int h);

// OpenGL uses unsigned integers to keep track of
// created resources (shaders, vertices, textures, etc)
// For simplicity, we make them global here, but it is
// safe to put them in a class and pass around...
GLuint gShaderProgram = 0;

//Shader for animated models
GLuint gShaderProgramAnim = 0;

//Particle shader
GLuint gShaderProgramPS = 0;

// full screen quad stuff
GLuint gVertexBufferFS = 0;
GLuint gVertexAttributeFS = 0;
GLuint gShaderProgramFS = 0;

//PF
GLuint gVertexBufferSM = 0;
GLuint gVertexAtrributeSM = 0;
GLuint gShaderProgramSM = 0;

float gClearColour[3] {};

//MVP-Matrix
GLint model_id = -1;
GLint model_id_anim = -1;
glm::mat4 model_matrix;
GLint view_id = -1;
GLint view_id_anim = -1;
glm::mat4 view_matrix;
GLint projection_id = -1;
GLint projection_id_anim = -1;
glm::mat4 projection_matrix;
GLuint shadow_id = -1; //PF
glm::mat4 shadow_matrix; //PF

//Camera variables
glm::vec3 camPos	= glm::vec3(0.5f, 0.5f, 8.0f); //Default camera position
glm::vec3 camFront	= glm::vec3(0.0f, 0.0f, -1.0f); //Default camera front
glm::vec3 camUp		= glm::vec3(0.0f, 1.0f, 0.0f); //Default camera up-vector
float FoV = 45.0f; //Field-of-view
float camYaw = -90.0f;
float camPitch = 0.0f;
float mouseLastX = WIDTH / 2; //At centre of the screen
float mouseLastY = HEIGHT / 2; //At centre of the screen

GameTimer timer;

// macro that returns "char*" with offset "i"
// BUFFER_OFFSET(5) transforms in "(char*)nullptr+(5)"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

unsigned int gFbo;
unsigned int gFboTextureAttachments[2]; // first for colour, second for depth
//const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
int CreateFrameBuffer() {
	int err = 0;
	// =================== COLOUR BUFFER =======================================
	// add "Attachments" to the framebuffer (textures to write to/read from)
	glGenTextures(2, gFboTextureAttachments);
	glBindTexture(GL_TEXTURE_2D, gFboTextureAttachments[0]);
	// define storage for texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	// define sampler settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// attach texture to framebuffer object

	// ===================== DEPTH BUFFER ==================================== 
	glBindTexture(GL_TEXTURE_2D, gFboTextureAttachments[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenFramebuffers(1, &gFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, gFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gFboTextureAttachments[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gFboTextureAttachments[1],0);

	// check if framebuffer is complete (usable):
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		err = 0; 
	} 
	else
		err = -1;

	// bind default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return err;
}

//PF
void CreateSMShaders() {
	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = 0;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	ifstream shaderFile("VertexShaderSM.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	const char* shaderTextPtr = shaderText.c_str();

	glShaderSource(vs, 1, &shaderTextPtr, nullptr);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {

		glGetShaderInfoLog(vs, 1024, nullptr, buff);

		OutputDebugStringA(buff);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("FragmentSM.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);

	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE) {

		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);

		OutputDebugStringA(buff);
	}

	gShaderProgramSM = glCreateProgram();
	glAttachShader(gShaderProgramSM, fs);
	glAttachShader(gShaderProgramSM, vs);
	glLinkProgram(gShaderProgramSM);

	compileResult = GL_FALSE;
	glGetProgramiv(gShaderProgramSM, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {

		memset(buff, 0, 1024);
		glGetProgramInfoLog(gShaderProgramSM, 1024, nullptr, buff);

		OutputDebugStringA(buff);
	}

	glDetachShader(gShaderProgramSM, vs);
	glDetachShader(gShaderProgramSM, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void CreateFSShaders() {
	// local buffer to store error strings when compiling.
	char buff[1024]; 
	memset(buff, 0, 1024);
	GLint compileResult = 0;

	//create vertex shader "name" and store it in "vs"
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	// open .glsl file and put it in a string
	ifstream shaderFile("VertexShaderFS.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	// glShaderSource requires a double pointer.
	// get the pointer to the c style string stored in the string object.
	const char* shaderTextPtr = shaderText.c_str();
	
	// ask GL to use this string as a shader code source
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);

	// try to compile this shader source.
	glCompileShader(vs);

	// check for compilation error
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		glGetShaderInfoLog(vs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	// repeat process for Fragment Shader (or Pixel Shader)
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("FragmentFS.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	// query information about the compilation (nothing if compilation went fine!)
	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	//link shader program (connect vs and ps)
	gShaderProgramFS = glCreateProgram();
	glAttachShader(gShaderProgramFS, fs);
	glAttachShader(gShaderProgramFS, vs);
	glLinkProgram(gShaderProgramFS);

	// check once more, if the Vertex Shader and the Fragment Shader can be used
	// together
	compileResult = GL_FALSE;
	glGetProgramiv(gShaderProgramFS, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetProgramInfoLog(gShaderProgramFS, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}
	// in any case (compile sucess or not), we only want to keep the 
	// Program around, not the shaders.
	glDetachShader(gShaderProgramFS, vs);
	glDetachShader(gShaderProgramFS, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void CreateShaders() {
	// local buffer to store error strings when compiling.
	char buff[1024]; 
	memset(buff, 0, 1024);
	GLint compileResult = 0;

	//create vertex shader "name" and store it in "vs"
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	// open .glsl file and put it in a string
	ifstream shaderFile("VertexShader.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	// glShaderSource requires a double pointer.
	// get the pointer to the c style string stored in the string object.
	const char* shaderTextPtr = shaderText.c_str();
	
	// ask GL to use this string a shader code source
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);

	// try to compile this shader source.
	glCompileShader(vs);

	// check for compilation error
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		glGetShaderInfoLog(vs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	// repeat process for Fragment Shader (or Pixel Shader)
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("Fragment.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	// query information about the compilation (nothing if compilation went fine!)
	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	// repeat process for Geometry Shader
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	shaderFile.open("GeometryShader.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(gs, 1, &shaderTextPtr, nullptr);
	glCompileShader(gs);
	// query information about the compilation (nothing if compilation went fine!)
	compileResult = GL_FALSE;
	glGetShaderiv(gs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetShaderInfoLog(gs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	//link shader program (connect vs, gs and ps)
	gShaderProgram = glCreateProgram();
	glAttachShader(gShaderProgram, fs);
	glAttachShader(gShaderProgram, vs);
	glAttachShader(gShaderProgram, gs);
	glLinkProgram(gShaderProgram);

	// check once more, if the Vertex Shader, Geometry Shader and the Fragment Shader can be used together
	compileResult = GL_FALSE;
	glGetProgramiv(gShaderProgram, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetProgramInfoLog(gShaderProgram, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}
	// in any case (compile sucess or not), we only want to keep the 
	// Program around, not the shaders.
	glDetachShader(gShaderProgram, vs);
	glDetachShader(gShaderProgram, fs);
	glDetachShader(gShaderProgram, gs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);
}

void CreateParticleShaders() {
	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = 0;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	ifstream shaderFile("VertexShaderPS.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	const char* shaderTextPtr = shaderText.c_str();
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		glGetShaderInfoLog(vs, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("FragmentShaderPS.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	shaderFile.open("GeometryShader.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(gs, 1, &shaderTextPtr, nullptr);
	glCompileShader(gs);
	compileResult = GL_FALSE;
	glGetShaderiv(gs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		memset(buff, 0, 1024);
		glGetShaderInfoLog(gs, 1024, nullptr, buff);
		OutputDebugStringA(buff);
	}

	gShaderProgramPS = glCreateProgram();
	glAttachShader(gShaderProgramPS, fs);
	glAttachShader(gShaderProgramPS, vs);
	glAttachShader(gShaderProgramPS, gs);
	glLinkProgram(gShaderProgramPS);

	compileResult = GL_FALSE;
	glGetProgramiv(gShaderProgramPS, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetProgramInfoLog(gShaderProgramPS, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}
	// in any case (compile sucess or not), we only want to keep the 
	// Program around, not the shaders.
	glDetachShader(gShaderProgramPS, vs);
	glDetachShader(gShaderProgramPS, fs);
	glDetachShader(gShaderProgramPS, gs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);
}

void CreateAnimShaders() {
	// local buffer to store error strings when compiling.
	char buff[1024];
	memset(buff, 0, 1024);
	GLint compileResult = 0;

	//create vertex shader "name" and store it in "vs"
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	// open .glsl file and put it in a string
	ifstream shaderFile("VertexShaderAnim.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();

	// glShaderSource requires a double pointer.
	// get the pointer to the c style string stored in the string object.
	const char* shaderTextPtr = shaderText.c_str();

	// ask GL to use this string a shader code source
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);

	// try to compile this shader source.
	glCompileShader(vs);

	// check for compilation error
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		glGetShaderInfoLog(vs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	// repeat process for Fragment Shader (or Pixel Shader)
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("FragmentAnim.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	// query information about the compilation (nothing if compilation went fine!)
	compileResult = GL_FALSE;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetShaderInfoLog(fs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	// repeat process for Geometry Shader
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	shaderFile.open("GeometryShaderAnim.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(gs, 1, &shaderTextPtr, nullptr);
	glCompileShader(gs);
	// query information about the compilation (nothing if compilation went fine!)
	compileResult = GL_FALSE;
	glGetShaderiv(gs, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetShaderInfoLog(gs, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}

	//link shader program (connect vs, gs and ps)
	gShaderProgramAnim = glCreateProgram();
	glAttachShader(gShaderProgramAnim, fs);
	glAttachShader(gShaderProgramAnim, vs);
	glAttachShader(gShaderProgramAnim, gs);
	glLinkProgram(gShaderProgramAnim);

	// check once more, if the Vertex Shader, Geometry Shader and the Fragment Shader can be used together
	compileResult = GL_FALSE;
	glGetProgramiv(gShaderProgramAnim, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE) {
		// query information about the compilation (nothing if compilation went fine!)
		memset(buff, 0, 1024);
		glGetProgramInfoLog(gShaderProgramAnim, 1024, nullptr, buff);
		// print to Visual Studio debug console output
		OutputDebugStringA(buff);
	}
	// in any case (compile sucess or not), we only want to keep the 
	// Program around, not the shaders.
	glDetachShader(gShaderProgramAnim, vs);
	glDetachShader(gShaderProgramAnim, fs);
	glDetachShader(gShaderProgramAnim, gs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);
}

void CreateFullScreenQuad() {
	struct Pos2UV {
		float x,y;
		float u,v;
	};
	Pos2UV myQuad[6] = {
		-1,-1, 0, 0,	// TOP		LEFT
		-1,+1, 0, 1,	// BOTTOM	LEFT
		+1,+1, 1, 1,	// BOTTOM	RIGHT
		-1,-1, 0, 0,	// TOP		LEFT
		+1,+1, 1, 1,	// BOTTOM	RIGHT
		+1,-1, 1, 0,	// TOP		RIGHT
	};

	// Vertex Array Object (VAO), description of the inputs to the GPU 
	glGenVertexArrays(1, &gVertexAttributeFS);
	// bind is like "enabling" the object to use it
	glBindVertexArray(gVertexAttributeFS);
	// this activates the first and second attributes of this VAO
	// think of "attributes" as inputs to the Vertex Shader
	glEnableVertexAttribArray(0); 
	glEnableVertexAttribArray(1);

	// create a vertex buffer object (VBO) id (out Array of Structs on the GPU side)
	glGenBuffers(1, &gVertexBufferFS);

	// Bind the buffer ID as an ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferFS);

	// This "could" imply copying to the GPU, depending on what the driver wants to do, and
	// the last argument (read the docs!)
	glBufferData(GL_ARRAY_BUFFER, sizeof(myQuad), myQuad, GL_STATIC_DRAW);

	// tell OpenGL about layout in memory (input assembler information)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Pos2UV), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Pos2UV), BUFFER_OFFSET(sizeof(float)*2));
};

GLuint CreateTexture(string path) {
	GLuint texture = 0;
	glGenTextures(1, &texture); //Generate the texture, first input is amt of textures, second is where we store them.
	glBindTexture(GL_TEXTURE_2D, texture); // We bind the genereated texture to be able to change it.

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//	First arg. is target, this case a 2D texture, second is setting we want to edit and axis,
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// last one is what texture wrapping mode we want to use. 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//Following two lines are for scaling setting it to use (bi)linear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, colourChannels;
	const char* filePath = path.c_str(); //Path to image file
	cout << "Texture path: " << filePath << endl; //Debug

	//The stbi_load funcions opens the file, and width and height values from the image and the amount of colour channels in the image.
	unsigned char* data = stbi_load(filePath, &width, &height, &colourChannels, 0); 
	if (data) {
		// function args. in order | Target | Mipmap | Image format | Width | Height | Legacy, need to be 0 | Format | Datatype | Image data | 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); // Generate the required mipmaps.
	}
	else {
		cout << "Failed to load texture. Reason: " << stbi_failure_reason() << endl; //Output error message if texture file is not found
	}
	stbi_image_free(data); // Free image memory
	return texture; //Returns an unsigned int/textureID
}

void CreateScene(Scene& scene) {
	//Fill the scene object with models to render
	scene.addModel("Resources/Models/ship.obj");
	scene.addModel("Resources/Models/cruiser.obj"); //Model borrowed from: http://www.prinmath.com/csci5229/OBJ/index.html
	scene.addModel("Resources/Models/plane.obj");

	scene.addAnimatedModel("Resources/Models/anim_test2.dae");
	//scene.addAnimatedModel("Resources/Models/anim_test2.dae");

	scene.addAnimatedModel("Resources/Models/model.dae");

	scene.prepareJoints(); //Important step! Get IDs for all joints

	for (int i = 0; i < scene.getModelCount(); i++) {
		//Create textures
		scene.models[i].setTextureID(CreateTexture(scene.models[i].getTexturePath()));

		//Create VAO and VBO
		scene.models[i].setVaoID(scene.CreateVAO());
		scene.models[i].setVboID(scene.CreateVBO());

		// This "could" imply copying to the GPU, depending on what the driver wants to do, and
		// the last argument (read the docs!)
		glBufferData(GL_ARRAY_BUFFER, scene.models[i].getVertCount() * VERTEX_SIZE, scene.models[i].vertices.data(), GL_STATIC_DRAW);

		// this activates the first, second and third attributes of this VAO
		// think of "attributes" as inputs to the Vertex Shader
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
		GLint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");
		// if this returns -1, it means there is a problem, and the program will likely crash.
		// examples: the name is different or missing in the shader

		if (vertexPos == -1) {
			OutputDebugStringA("Error, cannot find 'vertex_position' attribute in Vertex shader\n");
		}
		// tell OpenGL about layout in memory (input assembler information)
		glVertexAttribPointer(
			vertexPos,				// location in shader
			3,						// how many elements of type (see next argument)
			GL_FLOAT,				// type of each element
			GL_FALSE,				// integers will be normalized to [-1,1] or [0,1] when read...
			VERTEX_SIZE,			// distance between two vertices in memory (stride)
			BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
		);

		// repeat the process for the second attribute.
		// query which "slot" corresponds to the input texture_coords in the Vertex Shader 
		GLint textureCoord = glGetAttribLocation(gShaderProgram, "texture_coords");
		glVertexAttribPointer(textureCoord, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, BUFFER_OFFSET(sizeof(float) * 3));

		GLint normals = glGetAttribLocation(gShaderProgram, "normals");
		if (normals == -1) {
			OutputDebugStringA("Error, cannot find 'normals' attribute in Vertex shader\n");
		}
		glVertexAttribPointer(normals, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, BUFFER_OFFSET(sizeof(float) * 5));
	}

	for (int i = 0; i < scene.getAnimModelCount(); i++) {
		scene.animatedModels[i]->setTextureID(CreateTexture(scene.animatedModels[i]->getTexturePath())); //Create textures

		scene.animatedModels[i]->setVaoID(scene.CreateVAO());
		scene.animatedModels[i]->setVboID(scene.CreateVBO());

		glBufferData(GL_ARRAY_BUFFER, scene.animatedModels[i]->getVertCount() * ANIM_VERTEX_SIZE, scene.animatedModels[i]->vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		scene.animatedModels[i]->setIboID(scene.CreateIBO());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, scene.animatedModels[i]->numIndices * sizeof(unsigned int), scene.animatedModels[i]->indices.data(), GL_STATIC_DRAW);

		// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
		GLint vertexPos = glGetAttribLocation(gShaderProgramAnim, "vertex_position");
		GLint textureCoord = glGetAttribLocation(gShaderProgramAnim, "texture_coords");
		GLint normals = glGetAttribLocation(gShaderProgramAnim, "normals");

		// tell OpenGL about layout in memory (input assembler information)
		glVertexAttribPointer(vertexPos, 3,	GL_FLOAT, GL_FALSE, ANIM_VERTEX_SIZE, BUFFER_OFFSET(0));
		glVertexAttribPointer(textureCoord, 2, GL_FLOAT, GL_FALSE, ANIM_VERTEX_SIZE, BUFFER_OFFSET(sizeof(float) * 3));
		glVertexAttribPointer(normals, 3, GL_FLOAT, GL_FALSE, ANIM_VERTEX_SIZE, BUFFER_OFFSET(sizeof(float) * 5));

		scene.animatedModels[i]->setVboIDJoints(scene.CreateVBO());
		glBufferData(GL_ARRAY_BUFFER, scene.animatedModels[i]->perVertexJointData.size() * VERTEX_JOINT_DATA_SIZE, scene.animatedModels[i]->perVertexJointData.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		GLint jointIDs = glGetAttribLocation(gShaderProgramAnim, "joint_indices");
		GLint weights = glGetAttribLocation(gShaderProgramAnim, "weights");
		glVertexAttribIPointer(jointIDs, 4, GL_INT, VERTEX_JOINT_DATA_SIZE, BUFFER_OFFSET(0)); //Int pointer
		glVertexAttribPointer(weights, 4, GL_FLOAT, GL_FALSE, ANIM_VERTEX_SIZE, BUFFER_OFFSET(sizeof(unsigned int) * 4));

	}

	//Add lights
	scene.addLight(glm::vec3(4.0, 6.0, 2.0), glm::vec3(1.0f, 1.0f, 1.0f));
	scene.addLight(glm::vec3(-8.0, 6.0, 2.0), glm::vec3(1.0f, 0.0f, 0.0f)); //A red light

	scene.prepareLights(); //Important step! Assigns uniform IDs
}

void CreateMatrixData(GLuint shaderProg, GLint &projectionID, GLint &viewID) {
	projection_matrix = glm::perspective(glm::radians(FoV), WIDTH / HEIGHT, 0.1f, 100.0f);
	projectionID = glGetUniformLocation(shaderProg, "PROJ_MAT");
	if (projectionID == -1) {
		OutputDebugStringA("Error, cannot find 'projection_id' attribute in Geometry shader\n");
		return;
	}

	view_matrix = glm::lookAt(camPos, camPos + camFront, camUp); //Camera position, Looking direction and Up vector
	viewID = glGetUniformLocation(shaderProg, "VIEW_MAT");
	if (viewID == -1) {
		OutputDebugStringA("Error, cannot find 'view_id' attribute in Geometry shader\n");
		return;
	}
}

void CreateModelMatrix(float rotationValue, glm::vec3 translation, GLuint shaderProg, GLint &modelID) {
	glm::mat4 identity_mat = glm::mat4(1.0f);
	model_matrix = glm::translate(identity_mat, translation);
	model_matrix = glm::rotate(model_matrix, rotationValue, glm::vec3(0.0f, 1.0f, 0.0f));
	modelID = glGetUniformLocation(shaderProg, "MODEL_MAT");
	if (modelID == -1) {
		OutputDebugStringA("Error, cannot find 'model_id' attribute in Geometry shader\n");
		return;
	}
}

//PF
//void CreateShadowMatrixData(glm::vec3 lightPos) {
//
//	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
//	glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//	glm::mat4 depthModelMatrix = glm::mat4(1.0);
//	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
//
//	glm::mat4 shadowBias = glm::mat4(0.5, 0.0, 0.0, 0.0,
//			0.0, 0.5, 0.0, 0.0,
//			0.0, 0.0, 0.5, 0.0,
//			0.5, 0.5, 0.5, 1.0);
//
//
//	shadow_matrix = shadowBias * depthMVP;
//	shadow_id = glGetUniformLocation(gShaderProgramSM, "SHADOW_MAT");
//	if (shadow_id == -1) {
//		OutputDebugStringA("Error, cannot find 'shadow_id' attribute in Vertex shader SM\n");
//		return;
//	}
//}

void SetViewport() {
	// usually (not necessarily) this matches with the window size
	glViewport(0, 0, WIDTH, HEIGHT);
}

void Render(Scene& scene, float rotationVal) {
	// set the color TO BE used (this does not clear the screen right away)
	glClearColor(gClearColour[0], gClearColour[1],gClearColour[2],1.0);
	// use the color to clear the color buffer (clear the color buffer only)
	glClear(GL_COLOR_BUFFER_BIT);

	//Assign material data IDs for all models
	scene.prepareMaterials();

	//Choose model placement (default is origo)
	scene.models[1].setWorldPosition(glm::vec3(5.0f, 1.0f, 0.0f));
	scene.models[2].setWorldPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	scene.animatedModels[0]->setWorldPosition(glm::vec3(2.0f, 1.0f, -5.0f));
	scene.animatedModels[1]->setWorldPosition(glm::vec3(-6.0f, 2.0f, -2.5f));

	//Chose model rotations (default is 0.0f)
	scene.models[0].setWorldRotation(rotationVal);

	////////// Render static models //////////
	glUseProgram(gShaderProgram); //Choose a shader
	glUniformMatrix4fv(projection_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));  //Sends data about projection-matrix to geometry-shader
	glUniformMatrix4fv(view_id, 1, GL_FALSE, glm::value_ptr(view_matrix));				//Sends data about view-matrix to geometry-shader

	glUniform3fv(scene.lights_pos_id, scene.getLightCount(), glm::value_ptr(scene.lightPositions[0]));  //Sends light position data to fragment-shader
	glUniform3fv(scene.lights_color_id, scene.getLightCount(), glm::value_ptr(scene.lightColors[0]));   //Sends light color data to fragment-shader

	//Draws all static models in the scene
	for (int i = 0; i < scene.getModelCount(); i++) {
		//Send model matrix data per model
		CreateModelMatrix(scene.models[i].getWorldRotation(), scene.models[i].getWorldPosition(), gShaderProgram, model_id);  //Exchange rotation for "0.0f" to stop rotation
		glUniformMatrix4fv(model_id, 1, GL_FALSE, glm::value_ptr(model_matrix)); //Sends data about model-matrix to geometry-shader

		//Send texture data
		glActiveTexture(GL_TEXTURE0); //Activate the texture unit
		glBindTexture(GL_TEXTURE_2D, scene.models[i].getTextureID()); //Bind the texture

		glUniform3fv(scene.models[i].ambID, 1, glm::value_ptr(scene.models[i].ambientVal));		//Ambient
		glUniform3fv(scene.models[i].diffID, 1, glm::value_ptr(scene.models[i].diffuseVal));	//Diffuse
		glUniform3fv(scene.models[i].specID, 1, glm::value_ptr(scene.models[i].specularVal));	//Specular

		// tell opengl we are going to use the VAO we described earlier
		glBindVertexArray(scene.models[i].getVaoID());

		glDrawArrays(GL_TRIANGLES, 0, scene.models[i].getVertCount());
	}

	////////// Render animated models using another shader program //////////
	glUseProgram(gShaderProgramAnim); //Choose a shader
	glUniformMatrix4fv(projection_id_anim, 1, GL_FALSE, glm::value_ptr(projection_matrix));  //Sends data about projection-matrix to geometry-shader
	glUniformMatrix4fv(view_id_anim, 1, GL_FALSE, glm::value_ptr(view_matrix));				//Sends data about view-matrix to geometry-shader

	glUniform3fv(scene.anim_lights_pos_id, scene.getLightCount(), glm::value_ptr(scene.lightPositions[0]));  //Sends light position data to fragment-shader
	glUniform3fv(scene.anim_lights_color_id, scene.getLightCount(), glm::value_ptr(scene.lightColors[0]));   //Sends light color data to fragment-shader

	//Draws all animated models in the scene
	for (int i = 0; i < scene.getAnimModelCount(); i++) {
		CreateModelMatrix(scene.animatedModels[i]->getWorldRotation(), scene.animatedModels[i]->getWorldPosition(), gShaderProgramAnim, model_id_anim);
		glUniformMatrix4fv(model_id_anim, 1, GL_FALSE, glm::value_ptr(model_matrix));

		scene.animLoader.CalcJointTransform(timer.GetTimeSec(), scene.animatedModels[i]); //Calculate joint transforms
		for (int j = 0; j < scene.animatedModels[i]->jointCount; j++) {
			glUniformMatrix4fv(scene.animatedModels[i]->jointLocations[j], 1, GL_TRUE, (const GLfloat*)&scene.animatedModels[i]->jointTransforms[j]); //Send the matrices to the shader (the transpose is intentional)
		}

		//Send texture data
		glActiveTexture(GL_TEXTURE0); //Activate the texture unit
		glBindTexture(GL_TEXTURE_2D, scene.animatedModels[i]->getTextureID()); //Bind the texture

		glUniform3fv(scene.animatedModels[i]->ambID, 1, glm::value_ptr(scene.animatedModels[i]->ambientVal));	//Ambient
		glUniform3fv(scene.animatedModels[i]->diffID, 1, glm::value_ptr(scene.animatedModels[i]->diffuseVal));	//Diffuse
		glUniform3fv(scene.animatedModels[i]->specID, 1, glm::value_ptr(scene.animatedModels[i]->specularVal));	//Specular

		// tell opengl we are going to use the VAO we described earlier
		glBindVertexArray(scene.animatedModels[i]->getVaoID());

		glDrawElements(GL_TRIANGLES, scene.animatedModels[i]->numIndices, GL_UNSIGNED_INT, 0);
	}
}

void keyboardUpdate() {
	float movementSpeed = 2.5f * timer.GetDeltaTime();
	if (GetAsyncKeyState(GLFW_KEY_W)) //GetAsyncKeyState Determines whether a key is up or down at the time of the function call.
		camPos += movementSpeed * camFront;

	if (GetAsyncKeyState(GLFW_KEY_A))
		camPos -= glm::normalize(glm::cross(camFront, camUp)) * movementSpeed;

	if (GetAsyncKeyState(GLFW_KEY_S))
		camPos -= movementSpeed * camFront;

	if (GetAsyncKeyState(GLFW_KEY_D))
		camPos += glm::normalize(glm::cross(camFront, camUp)) * movementSpeed;
}

//This function uses output from the mouse wheel
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	FoV = FoV - 5 * yOffset;
	if (FoV <= 10.0f) {
		FoV = 10.0f;
	}
	if (FoV >= 100.0f) {
		FoV = 100.0f;
	}
	cout << "Current FoV: " << FoV << endl;
}

void mouseCallback(GLFWwindow* window, double xPos, double yPos) {
	float mouseSpeed = 0.05f;

	float xOffset = xPos - mouseLastX;
	float yOffset = mouseLastY - yPos;
	mouseLastX = xPos;
	mouseLastY = yPos;

	xOffset *= mouseSpeed;
	yOffset *= mouseSpeed;

	camYaw += xOffset;
	camPitch += yOffset;

	//Constraining camera movement to avoid flipping
	if (camPitch >= 90.0f) {
		camPitch = 90.0f;;
	}
	if (camPitch <= -90.0f) {
		camPitch = -90.0f;
	}

	//Calculating direction vector
	glm::vec3 direction = glm::vec3(
		cos(glm::radians(camPitch)) * cos(glm::radians(camYaw)),
		sin(glm::radians(camPitch)),
		cos(glm::radians(camPitch)) * sin(glm::radians(camYaw)));
	camFront = glm::normalize(direction);

	//This provides feedback to console for mouse movement
	//cout << "X-Position: " << xPos << endl;
	//cout << "Y-Position: " << yPos << endl;
	//cout << endl;
}

//This function specifies the layout of debug messages
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	//Take out 131185 for example to test debug messages
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) { //Insignificant errors/notifications
		return;
	}

	freopen("CON", "w", stdout); //Redirects the string stream to the debug console

	cout << "---------------" << endl;
	cout << "Debug message (" << id << "): " << message << endl;

	switch (source) {
	case GL_DEBUG_SOURCE_API:             cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           cout << "Source: Other"; break;
	} cout << endl;

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:               cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               cout << "Type: Other"; break;
	} cout << endl;

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:         cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: cout << "Severity: notification"; break;
	} cout << endl;
	cout << endl;
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow ) {
	//Attaches a console used for displaying debug messages to the application
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout); //Redirects the string stream to the debug console
	cout << "Welcome to the debug console!" << endl;

	initWindow(WIDTH, HEIGHT);
	bool shutdown = false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(gWindow, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGui::StyleColorsDark();

	//Enable OpenGL debug context if context allows for debug context
	GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //Makes sure errors are displayed synchronously
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	CreateShaders(); //5. Create vertex- and fragment-shaders
	CreateAnimShaders(); //5. Create shaders for animated models
	CreateParticleShaders(); //5.
	CreateFSShaders(); //5. Create vertex- and fragment-shaders
	//CreateSMShaders(); //PF

	if (CreateFrameBuffer() != 0)
		shutdown = true;

	//Create a scene object and fill it
	Scene gameScene(gShaderProgram, gShaderProgramAnim);
	CreateScene(gameScene);

	CreateFullScreenQuad();

	float rotation = 0.0f;

	while (!glfwWindowShouldClose(gWindow)) {
		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(gWindow, 1);
		}


		//Calculate delta time
		timer.Tick();

		float increment = 0.6f * timer.GetDeltaTime();

		if (rotation >= _360_DEGREES) {
			rotation -= _360_DEGREES;
		}

		rotation += increment;

		keyboardUpdate();

		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// first pass
		// render all geometry to a framebuffer object
		glBindFramebuffer(GL_FRAMEBUFFER, gFbo);
		glClearColor(gClearColour[0], gClearColour[1], gClearColour[2], gClearColour[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		//Prepare IMGUI output
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		ImGui::Begin("Debug");                          // Create a window called "Debug" and append into it.
		ImGui::Text("DV1568 3D-Programming");           // Display some text (you can use a format strings too)   
		ImGui::ColorEdit3("clear color", gClearColour); // Edit 3 floats representing a color
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		static float scale = 1.0f;
		ImGui::SliderFloat("Scale", &scale, 0.0f, 1.0f);
		static bool renderDepth = false;
		ImGui::Checkbox("Show DepthMap", &renderDepth);
		ImGui::End();

		CreateMatrixData(gShaderProgram, projection_id, view_id); //Creates mvp-matrix for normal shader
		CreateMatrixData(gShaderProgramAnim, projection_id_anim, view_id_anim); //Creates mvp-matrix for FBX-shader

		/*GLuint depthMatrixID = -1;*/
		//glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &shadowBiasMVP[0][0]); 
		/*glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, glm::value_ptr(shadowBiasMVP));*/

		Render(gameScene, rotation); //9. Render

		// first pass is done!
		// now render a second pass
		// bind default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(gShaderProgramFS);
		glBindVertexArray(gVertexAttributeFS);

		glDisable(GL_DEPTH_TEST);

		// bind texture drawn in the first pass!
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gFboTextureAttachments[0]);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, gFboTextureAttachments[1]);
		
		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
		glm::mat4 transform = scaleMat;
		glUniformMatrix4fv(5, 1, GL_TRUE, &transform[0][0]);

		// false
		glUniform1i(3, renderDepth);  // 0 == false
		
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw quad

		//Render IMGUI interface
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(gWindow); //10. Växla front- och back-buffer
	}

	// SHUTDOWN
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glDeleteFramebuffers(1, &gFbo);
	glDeleteTextures(2, gFboTextureAttachments);
	//gameScene.cleanup(); //Deletes all Animated Models in the scene
	gameScene.deleteVAOs(); //Deletes all vaos in the scene
	gameScene.deleteVBOs(); //Deletes all vbos in the scene
	gameScene.deleteIBOs(); //Deletes all index buffers in the scene
	glDeleteVertexArrays(1, &gVertexAttributeFS);
	glDeleteBuffers(1, &gVertexBufferFS);
	glfwTerminate();

	return 0;

}

void initWindow(unsigned int w, unsigned int h) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); //Allows for Debug output (should generally be commented out in a release build)
	//glfwWindowHint(GLFW_SAMPLES, 4);

	gWindow = glfwCreateWindow(w, h, "DV1568 3D-Programming Project", NULL, NULL);
	if (!gWindow) {
		fprintf(stderr, "error creating window\n");
		exit(-1);
	}
	glfwMakeContextCurrent(gWindow);
	glewExperimental = GL_TRUE;

	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Disables mouse cursor (Press ESCAPE to quit application)
	glfwSetCursorPos(gWindow, mouseLastX, mouseLastY); //Initializes cursor position to the middle of the screen
	glfwSetScrollCallback(gWindow, scrollCallback); //Scroll-wheel callback
	glfwSetCursorPosCallback(gWindow, mouseCallback); //Mouse callback

	glfwSwapInterval(1);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "Error GLEW: %s\n", glewGetErrorString(err));
	}
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	fprintf(stderr, "Renderer: %s\n", renderer);
	fprintf(stderr, "OpenGL version %s\n", version);

	// start up time
	// timerStart = glfwGetTime();
	glClearColor(0.9f, 0.2f, 0.2f, 0.0f);
	glClearDepth(1.0f);
	glViewport(0, 0, w, h);

	return;
}


