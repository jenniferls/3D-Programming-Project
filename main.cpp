//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//     - updated by FLL
//--------------------------------------------------------------------------------------
#include <windows.h>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

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

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")

#define WIDTH 900.0f
#define HEIGHT 900.0f
GLFWwindow *gWindow;

using namespace std;

void initWindow(unsigned int w, unsigned int h);

// OpenGL uses unsigned integers to keep track of
// created resources (shaders, vertices, textures, etc)
// For simplicity, we make them global here, but it is
// safe to put them in a class and pass around...
GLuint gVertexBuffer = 0;
GLuint gVertexAttribute = 0;
GLuint gShaderProgram = 0;

// full screen quad stuff
GLuint gVertexBufferFS = 0;
GLuint gVertexAttributeFS = 0;
GLuint gShaderProgramFS = 0;

float gClearColour[3] {};

GLint mvp_id = -1;
glm::mat4 mvp_matrix;
float FoV = 45.0f; //Field-of-view

// macro that returns "char*" with offset "i"
// BUFFER_OFFSET(5) transforms in "(char*)nullptr+(5)"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

unsigned int gFbo;
unsigned int gFboTextureAttachments[2]; // first for colour, second for depth
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

	//link shader program (connect vs and ps)
	gShaderProgram = glCreateProgram();
	glAttachShader(gShaderProgram, fs);
	glAttachShader(gShaderProgram, vs);
	glLinkProgram(gShaderProgram);

	// check once more, if the Vertex Shader and the Fragment Shader can be used
	// together
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
	glDeleteShader(vs);
	glDeleteShader(fs);
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

void CreateTriangleData() {
	// this is how we will structure the input data for the vertex shader
	// every six floats, is one vertex.
	struct TriangleVertex {
		float x, y, z;
		float r, g, b;
		float myAttr;
	};

	// create the actual data in plane Z = 0
	// This is called an Array of Structs (AoS) because we will 
	// end up with an array of many of these structs.
	TriangleVertex triangleVertices[3] = {
		// pos and color for each vertex
		{ 0.0f, 0.5f, 0.3f,	1.0f, 0.0f, 0.0f, 0.0},
		{ 0.5f, -0.5f, 0.3f, 0.0f, 1.0f, 0.0f, 0.0},
		{ -0.5f, -0.5f, 0.3f, 0.0f, 0.0f, 1.0f, 0.0}
	};

	// Vertex Array Object (VAO), description of the inputs to the GPU 
	glGenVertexArrays(1, &gVertexAttribute);
	// bind is like "enabling" the object to use it
	glBindVertexArray(gVertexAttribute);
	// this activates the first and second attributes of this VAO
	// think of "attributes" as inputs to the Vertex Shader
	glEnableVertexAttribArray(0); 
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// create a vertex buffer object (VBO) id (out Array of Structs on the GPU side)
	glGenBuffers(1, &gVertexBuffer);

	// Bind the buffer ID as an ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);

	// This "could" imply copying to the GPU, depending on what the driver wants to do, and
	// the last argument (read the docs!)
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	// query which "slot" corresponds to the input vertex_position in the Vertex Shader 
	GLint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");
	// if this returns -1, it means there is a problem, and the program will likely crash.
	// examples: the name is different or missing in the shader

	if (vertexPos == -1) {
		OutputDebugStringA("Error, cannot find 'vertex_position' attribute in Vertex shader\n");
		return;
	}

	// tell OpenGL about layout in memory (input assembler information)
	glVertexAttribPointer(
		vertexPos,				// location in shader
		3,						// how many elements of type (see next argument)
		GL_FLOAT,				// type of each element
		GL_FALSE,				// integers will be normalized to [-1,1] or [0,1] when read...
		sizeof(TriangleVertex), // distance between two vertices in memory (stride)
		BUFFER_OFFSET(0)		// offset of FIRST vertex in the list.
	);

	// repeat the process for the second attribute.
	// query which "slot" corresponds to the input vertex_color in the Vertex Shader 
	GLuint vertexColor = glGetAttribLocation(gShaderProgram, "vertex_color");
	glVertexAttribPointer(
		vertexColor, 
		3, 
		GL_FLOAT, 
		GL_FALSE, sizeof(TriangleVertex), // distance between two vertexColor 
		BUFFER_OFFSET(sizeof(float)*3)	// note, the first color starts after the first vertex.
	);

	GLint myAttrLoc = glGetAttribLocation(gShaderProgram, "myAttr");
	glVertexAttribPointer(myAttrLoc, 1, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float)*6));
}

void CreateMatrixData(float FoV) {
	//MVP-matrix
	const glm::mat4 world = glm::mat4(1.0f);
	const glm::mat4 view = glm::lookAt(
		glm::vec3(0, 0, 2), //Camera position
		glm::vec3(0, 0, 0), //Looks at origin
		glm::vec3(0, 1, 0)  //Up vector
	);
	const glm::mat4 projection = glm::perspective(glm::radians(FoV), WIDTH / HEIGHT, 0.1f, 20.0f);
	mvp_matrix = projection * view * world;
	mvp_id = glGetUniformLocation(gShaderProgram, "MVP_MAT");
	if (mvp_id == -1) {
		OutputDebugStringA("Error, cannot find 'mvp_id' attribute in Vertex shader\n");
		return;
	}
}

void SetViewport() {
	// usually (not necessarily) this matches with the window size
	glViewport(0, 0, WIDTH, HEIGHT);
}

void Render() {
	// set the color TO BE used (this does not clear the screen right away)
	glClearColor(gClearColour[0], gClearColour[1],gClearColour[2],1.0);

	// use the color to clear the color buffer (clear the color buffer only)
	glClear(GL_COLOR_BUFFER_BIT);

	// tell opengl we want to use the gShaderProgram
	glUseProgram(gShaderProgram);

	// tell opengl we are going to use the VAO we described earlier
	glBindVertexArray(gVertexAttribute);
	
	// ask OpenGL to draw 3 vertices starting from index 0 in the vertex array 
	// currently bound (VAO), with current in-use shader. Use TOPOLOGY GL_TRIANGLES,
	// so for one triangle we need 3 vertices!
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/*
	if (key == GLFW_KEY_W)
		keys[0] = (action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_A)
		keys[1] = (action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_S)
		keys[2] = (action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_D)
		keys[3] = (action == GLFW_PRESS || action == GLFW_REPEAT);
	*/
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	freopen("CON", "w", stdout); //Redirects the string stream to the debug console
	cout << yOffset << endl;
	FoV = FoV - 5 * yOffset;
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
	glfwSetKeyCallback(gWindow, keyboard);
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
	CreateFSShaders(); //5. Create vertex- and fragment-shaders

	if (CreateFrameBuffer() != 0)
		shutdown = true;

	CreateTriangleData(); //6. Definiera triangelvertiser, 7. Skapa vertex buffer object (VBO), 8.Skapa vertex array object (VAO)
	CreateFullScreenQuad();

	while (!glfwWindowShouldClose(gWindow)) {
		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(gWindow, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(gWindow, 1);
		}

		glfwSetScrollCallback(gWindow, scrollCallback);

		// first pass
		// render all geometry to a framebuffer object
		glBindFramebuffer(GL_FRAMEBUFFER, gFbo);
		glClearColor(gClearColour[0], gClearColour[1], gClearColour[2], gClearColour[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(gShaderProgram);
		glBindVertexArray(gVertexAttribute);

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

		CreateMatrixData(FoV); //Creates mvp-matrix
		glUniformMatrix4fv(mvp_id, 1, GL_TRUE, glm::value_ptr(mvp_matrix)); //Sends data about mvp-matrix to vertex-shader

		Render(); //9. Render

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
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Render IMGUI interface
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(gWindow); //10. V�xla front- och back-buffer
	}

	// SHUTDOWN
	ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glDeleteFramebuffers(1, &gFbo);
	glDeleteTextures(2, gFboTextureAttachments);
	glDeleteVertexArrays(1, &gVertexAttribute);
	glDeleteVertexArrays(1, &gVertexAttributeFS);
	glDeleteBuffers(1, &gVertexBuffer);
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

	gWindow = glfwCreateWindow(w, h, "DV1568 3D-Programming Project", NULL, NULL);
	if (!gWindow) {
		fprintf(stderr, "error creating window\n");
		exit(-1);
	}
	glfwMakeContextCurrent(gWindow);
	glewExperimental = GL_TRUE;

	// mouse callback
	// glfwSetCursorPosCallback(gWindow, mouseMoved);

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


