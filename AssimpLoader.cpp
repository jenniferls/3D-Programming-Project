#include "AssimpLoader.h"
#include <windows.h>

AssimpLoader::AssimpLoader() {

}

AssimpLoader::~AssimpLoader() {

}

bool AssimpLoader::LoadModel(RawModel &model) {
	const aiScene* scene;
	scene = importer.ReadFile(model.getPath(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights); //Path and post processing flags
	//if (scene == NULL || scene->mNumAnimations == 0) {
	//	OutputDebugStringA("Error loading animated model!");
	//	return false;
	//}
	
	RawModel::TriangleVertex temp;
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];
		//model.positions = mesh->mVertices;
	}
	model.vertices.push_back(temp);
	return true;
}
