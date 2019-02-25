#pragma once
#include <assimp/include/assimp/Importer.hpp>
#include <assimp/include/assimp/scene.h>
#include <assimp/include/assimp/postprocess.h>
#include "AnimatedModel.h"
#include <vector>

class AssimpLoader {
public:
	AssimpLoader();
	~AssimpLoader();

	bool LoadModel(AnimatedModel &model);

private:
	Assimp::Importer importer;
};

