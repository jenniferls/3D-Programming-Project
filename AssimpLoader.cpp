#include "AssimpLoader.h"
#include <windows.h>
#include <string>

AssimpLoader::AssimpLoader() {

}

AssimpLoader::~AssimpLoader() {

}

bool AssimpLoader::LoadModel(AnimatedModel &model) {
	const aiScene* scene;
	scene = importer.ReadFile(model.getPath(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_JoinIdenticalVertices); //Path and post processing flags
	if (scene) {
		//m_GlobalInverseTransform = scene->mRootNode->mTransformation;
		//m_GlobalInverseTransform.Inverse();

		AnimatedModel::Vertex temp; //Temporary vertex for storing data
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* mesh = scene->mMeshes[i];
			const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			aiColor3D diff, amb, spec;
			material->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
			material->Get(AI_MATKEY_COLOR_AMBIENT, amb);
			material->Get(AI_MATKEY_COLOR_SPECULAR, spec);
			model.diffuseVal = glm::vec3(diff.r, diff.g, diff.b);
			model.ambientVal = glm::vec3(amb.r, amb.g, amb.b);
			model.specularVal = glm::vec3(spec.r, spec.g, spec.b);

			aiString path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL);
			std::string fileName = path.data;
			std::string fullPath = "Resources/Textures/" + fileName;
			model.setTexturePath(fullPath);

			for (int j = 0; j < mesh->mNumVertices; j++) {
				model.positions.push_back(glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));
				model.normals.push_back(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z));
				model.uvs.push_back(glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y));
				temp = { model.positions[j], model.uvs[j], model.normals[j] };
				model.vertices.push_back(temp);
			}

			for (int i = 0; i < mesh->mNumFaces; i++) {
				if (mesh->mFaces[i].mNumIndices == 3) {
					model.indices.push_back(mesh->mFaces[i].mIndices[0]);
					model.indices.push_back(mesh->mFaces[i].mIndices[1]);
					model.indices.push_back(mesh->mFaces[i].mIndices[2]);
				}
				else {
					OutputDebugStringA("ASSIMP: Model-loading failure! Mesh is not triangulated.");
					return false;
				}
			}
			model.numIndices = model.indices.size();
			model.setVertCount(model.vertices.size());
		}
	}
	else {
		OutputDebugStringA(importer.GetErrorString());
		return false;
	}

	return true;
}
