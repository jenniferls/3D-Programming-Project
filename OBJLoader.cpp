#include "OBJLoader.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <sstream>

OBJLoader::OBJLoader() {

}


OBJLoader::~OBJLoader() {

}

void OBJLoader::loadOBJ(const char * filename, std::vector<glm::vec3>& verts, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs, int & elements) {
	std::ifstream in(filename, std::fstream::in);
	if (!in) {
		OutputDebugStringA("Cannot load obj-file!");
	}
	std::string line;
	while (getline(in, line)) {
		if (line.substr(0, 2) == "v ") {
			std::istringstream s(line.substr(2));
			glm::vec3 v;
			s >> v.x;
			s >> v.y;
			s >> v.z;
			verts.push_back(v);
		}
	}
}
