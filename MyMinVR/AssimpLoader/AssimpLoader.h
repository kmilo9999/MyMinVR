#ifndef AILOADER_H
#define AILOADER_H

#include "../common/common.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

class Model;

class AssimpLoader
{

public:
	
	~AssimpLoader();

	static AssimpLoader* GetInstance();
	std::vector<Model*> loadFile(std::string filePath);

	void ProcessMesh(aiMesh* mesh, const aiScene* scene, std::vector<Model*>& models, GLenum mode);
	void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Model*>& models,GLenum mode);

	
private:

	AssimpLoader();
	const aiScene* myScene ;
	GLuint mySceneList;
	aiVector3D myScenemin;
	aiVector3D myScenemax;
	aiVector3D myScenecenter;
	static AssimpLoader* instance;
};

#endif