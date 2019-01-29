#include "OBJLoader.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <sstream>

OBJLoader::OBJLoader() : model(0, 0, ""){

}


OBJLoader::~OBJLoader() {

}

bool OBJLoader::loadOBJ(const char * filename, std::vector<glm::vec3> &vertPositions, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &uvs,
	std::vector<unsigned short> &vert_indices, std::vector<unsigned short> &uv_indices, std::vector<unsigned short> &norm_indices) {
	std::ifstream in(filename, std::fstream::in);
	if (!in) {
		OutputDebugStringA("Cannot load obj-file!");
		return false;
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
			unsigned short a, b, c, d, e, f, g, h, i;
			s >> a;
			s.ignore(line.length(), '/');
			s >> b;
			s.ignore(line.length(), '/');
			s >> c;

			s >> d;
			s.ignore(line.length(), '/');
			s >> e;
			s.ignore(line.length(), '/');
			s >> f;

			s >> g;
			s.ignore(line.length(), '/');
			s >> h;
			s.ignore(line.length(), '/');
			s >> i;

			vert_indices.push_back(a - 1);
			vert_indices.push_back(d - 1);
			vert_indices.push_back(g - 1);

			uv_indices.push_back(b - 1);
			uv_indices.push_back(e - 1);
			uv_indices.push_back(h - 1);

			norm_indices.push_back(c - 1);
			norm_indices.push_back(f - 1);
			norm_indices.push_back(i - 1);
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
	return true;
}
