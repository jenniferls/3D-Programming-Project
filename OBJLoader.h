#pragma once

#include <vector>
#include "glm/glm.hpp"

class OBJLoader {
public:
	OBJLoader();
	~OBJLoader();

	void loadOBJ(const char* filename, std::vector<glm::vec3> &verts, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &uvs, int &elements);

private:

};

