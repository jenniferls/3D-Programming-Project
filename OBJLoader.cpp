#include "OBJLoader.h"
#include <windows.h>
#include "glew/include/GL/glew.h"
#include <gl/GL.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <sstream>

OBJLoader::OBJLoader() {

}


OBJLoader::~OBJLoader() {

}

bool OBJLoader::loadOBJ(RawModel *model) {
	std::ifstream in(model->getPath(), std::fstream::in);
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
			model->positions.push_back(v);
		}
		else if (line.substr(0, 3) == "vn ") {
			std::istringstream s(line.substr(2));
			glm::vec3 n;
			s >> n.x;
			s >> n.y;
			s >> n.z;
			model->normals.push_back(n);
		}
		else if (line.substr(0, 3) == "vt ") {
			std::istringstream s(line.substr(2));
			glm::vec2 uv;
			s >> uv.x;
			s >> uv.y;
			model->uvs.push_back(uv);
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

			model->vertex_indices.push_back(a - 1);
			model->vertex_indices.push_back(d - 1);
			model->vertex_indices.push_back(g - 1);

			model->uv_indices.push_back(b - 1);
			model->uv_indices.push_back(e - 1);
			model->uv_indices.push_back(h - 1);

			model->normal_indices.push_back(c - 1);
			model->normal_indices.push_back(f - 1);
			model->normal_indices.push_back(i - 1);
		}
		else if (line.substr(0, 7) == "mtllib ") {
			std::istringstream s(line.substr(7));
			std::string matPath;
			s >> matPath;
			matPath = "Resources/Models/" + matPath;
			model->setMaterialPath(matPath);
			//OutputDebugStringA(model->getMaterialPath().c_str()); //For debug
		}
	}
	model->setVertCount(model->vertex_indices.size());
	for (int i = 0; i < model->getVertCount(); i++) {
		RawModel::TriangleVertex temp;
		temp = { model->positions[model->vertex_indices[i]], model->uvs[model->uv_indices[i]], model->normals[model->normal_indices[i]] };
		model->vertices.push_back(temp);
	}

	int amtOfVerts = model->getVertCount();
	glm::vec3 aTangent, aBitangent, normal = glm::vec3(0.0f);
	float vtxVectorX, vtxVectorY, vtxVectorZ = 0.0f;
	float U1, V1, U2, V2 = 0.0f;
	glm::vec3 edge1, edge2 = glm::vec3(0.0f);

	for (int i = 0; i < amtOfVerts; i += 3) {
		vtxVectorX = model->vertices[i].positions.x - model->vertices[i + 2].positions.x;
		vtxVectorY = model->vertices[i].positions.y - model->vertices[i + 2].positions.y;
		vtxVectorZ = model->vertices[i].positions.z - model->vertices[i + 2].positions.z;
		edge1 = glm::vec3(vtxVectorX, vtxVectorY, vtxVectorZ);

		vtxVectorX = model->vertices[i + 2].positions.x - model->vertices[i + 1].positions.x;
		vtxVectorY = model->vertices[i + 2].positions.y - model->vertices[i + 1].positions.y;
		vtxVectorZ = model->vertices[i + 2].positions.z - model->vertices[i + 1].positions.z;
		edge2 = glm::vec3(vtxVectorX, vtxVectorY, vtxVectorZ);

		normal = glm::cross(edge1, edge2);
		normal = normalize(normal);

		U1 = model->vertices[i].UVs.x - model->vertices[i + 2].UVs.x;
		V1 = model->vertices[i].UVs.y - model->vertices[i + 2].UVs.y;
		U2 = model->vertices[i + 2].UVs.x - model->vertices[i + 1].UVs.x;
		V2 = model->vertices[i + 2].UVs.y - model->vertices[i + 1].UVs.y;

		float f = 1.0f / (U1 * V2 - U2 * V1);
		aTangent.x = f * (U1 * edge1.x - V2 * edge2.x);
		aTangent.y = f * (U1 * edge1.y - V2 * edge2.y);
		aTangent.z = f * (U1 * edge1.z - V2 * edge2.z);
		aTangent = glm::normalize(aTangent);

	/*	model->vertices[i].normals = normal;
		model->vertices[i + 1].normals = normal;
		model->vertices[i + 2].normals = normal;*/

		model->vertices[i].tangents = aTangent;
		model->vertices[i + 1].tangents = aTangent;
		model->vertices[i + 2].tangents = aTangent;

		aBitangent = glm::cross(model->vertices[i].normals, aTangent);
		model->vertices[i].biTangents = aBitangent;
		model->vertices[i + 1].biTangents = aBitangent;
		model->vertices[i + 2].biTangents = aBitangent;
	}
	std::cout << "Tangent calculations done" << std::endl;

	return true;
}

bool OBJLoader::loadMTL(RawModel *model) {
	//OutputDebugStringA(model->getMaterialPath().c_str()); //For debug
	std::ifstream in(model->getMaterialPath(), std::fstream::in);
	if (!in) {
		OutputDebugStringA("Cannot load mtl-file!");
		return false;
	}

	std::string line;
	while (getline(in, line)) {
		if (line.substr(0, 3) == "Kd ") {
			std::istringstream s(line.substr(2));
			glm::vec3 diffuse;
			s >> diffuse.x;
			s >> diffuse.y;
			s >> diffuse.z;
			model->diffuseVal = { diffuse.x, diffuse.y, diffuse.z };
		}
		else if (line.substr(0, 3) == "Ks ") {
			std::istringstream s(line.substr(2));
			glm::vec3 spec;
			s >> spec.x;
			s >> spec.y;
			s >> spec.z;
			model->specularVal = { spec.x, spec.y, spec.z };
		}
		else if (line.substr(0, 3) == "Ka ") {
			std::istringstream s(line.substr(2));
			glm::vec3 ambient;
			s >> ambient.x;
			s >> ambient.y;
			s >> ambient.z;
			model->ambientVal = { ambient.x, ambient.y, ambient.z };
		}
		else if (line.substr(0, 7) == "map_Kd ") {
			std::istringstream s(line.substr(7));
			std::string tex;
			s >> tex;
			tex = "Resources/Textures/" + tex;
			model->setTexturePath(tex);
		}
		else if (line.substr(0, 12) == "bump  -bm 1 ") {
			std::istringstream s(line.substr(12));
			std::string tex;
			s >> tex;
			tex = "Resources/Textures/" + tex;
			model->setNormalTexturePath(tex);
			model->setHasNormal(true);
		}
	}
	return true;
}