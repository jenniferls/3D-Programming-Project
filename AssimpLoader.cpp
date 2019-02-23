#include "AssimpLoader.h"
#include <windows.h>

AssimpLoader::AssimpLoader() {

}

AssimpLoader::~AssimpLoader() {

}

bool AssimpLoader::LoadModel(AnimatedModel &model) {
	const aiScene* scene;
	scene = importer.ReadFile(model.getPath(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights); //Path and post processing flags
	if (scene) {
		//m_GlobalInverseTransform = scene->mRootNode->mTransformation;
		//m_GlobalInverseTransform.Inverse();

		AnimatedModel::Vertex temp; //Temporary vertex for storing data
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* mesh = scene->mMeshes[i];

			for (int j = 0; j < mesh->mNumVertices; j++) {
				model.positions.push_back(glm::vec3(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));
				model.normals.push_back(glm::vec3(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z));
				model.uvs.push_back(glm::vec2(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y));
				temp = { model.positions[j], model.uvs[j], model.normals[j] };
				model.vertices.push_back(temp);
			}
			model.setVertCount(model.vertices.size());

			//for (int i = 0; i < mesh->mNumFaces; i++) {
			//	temp = { model.positions[model.vertex_indices[i]], model.uvs[model.uv_indices[i]], model.normals[model.normal_indices[i]] };
			//	model.vertices.push_back(temp);
			//}
		}
	}
	else {
		OutputDebugStringA(importer.GetErrorString());
		return false;
	}

	return true;
}
