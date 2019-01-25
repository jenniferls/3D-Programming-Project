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

void OBJLoader::loadOBJ(const char * filename, std::vector<glm::vec3> &vertPositions, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &uvs, std::vector<unsigned short> &elements) {
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
			vertPositions.push_back(v);
		}
		else if (line.substr(0, 2) == "f ") {
			std::istringstream s(line.substr(2));
			unsigned short a, b, c;
			s >> a;
			s.ignore(line.length(), '/');
			s >> b;
			s.ignore(line.length(), '/');
			s >> c;
			a--; //since we need to start from 0
			b--; //since we need to start from 0
			c--; //since we need to start from 0
			elements.push_back(a);
			elements.push_back(b);
			elements.push_back(c);
		}
		else if (line.substr(0, 3) == "vn ") {
			std::istringstream s(line.substr(2));
			glm::vec3 n;
			s >> n.x;
			s >> n.y;
			s >> n.z;
			normals.push_back(n);
		}
		else if (line.substr(0, 3) == "vt ") {
			std::istringstream s(line.substr(2));
			glm::vec2 uv;
			s >> uv.x;
			s >> uv.y;
			uvs.push_back(uv);
		}
	}
}
