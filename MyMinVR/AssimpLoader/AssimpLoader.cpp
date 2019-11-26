#include "AssimpLoader.h"
#include "../GLMLoader/VertexBuffer.h"
#include "../Model/Model.h"


AssimpLoader* AssimpLoader::instance(0);


AssimpLoader::AssimpLoader():myScene(0), mySceneList(0)
{
}


AssimpLoader::~AssimpLoader()
{
}

AssimpLoader* AssimpLoader::GetInstance()
{
	if (!instance)
	{
		instance = new AssimpLoader();
	}
	return instance;
}

std::vector<Model*>  AssimpLoader::loadFile(std::string filePath)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	myScene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality);
	std::vector<Model*> models;

	// If the import failed, report it
	if (!myScene)
	{
		std::cout << importer.GetErrorString() << "\n";
	}
	else
	{
		ProcessNode(myScene->mRootNode, myScene, models, GL_TRIANGLES);
		std::cout << "Import of scene " + filePath + " succeeded \n";
	}
	
	return models;
}

void  AssimpLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::vector<Model*>& models, GLenum mode)
{
	// Data to fill
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> colors;
	std::vector<unsigned int> indices;
	// Walk through each of the mesh's vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
					 // Positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;

		vertices.push_back(vector);

		if (mesh->HasNormals())
		{
			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;

			normals.push_back(normal);
		}
	

		// Texture Coordinates
		glm::vec2 vec;
		if (mesh->mTextureCoords[0]) 
		{

			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;

		}
		else {
			vec.x = 0.0f;
			vec.y = 0.0f;
		}
		uv.push_back(vec);

		for (int j(0);j < AI_MAX_NUMBER_OF_COLOR_SETS;j++) 
		{
			if (mesh->HasVertexColors(0))
			{
				glm::vec3 color;
				color.x = mesh->mColors[0][i].r;
				color.y = mesh->mColors[0][i].g;
				color.z = mesh->mColors[0][i].b;
				colors.push_back(color);
			}
		}
		
		
	}

	
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}

	}

	VertexBuffer* vertexObject = new VertexBuffer();

	vertexObject->setData(vertices, normals, uv, indices);
	if (!colors.empty())
	{
		vertexObject->setColorsBuffer(colors);
	}

	// return model
	Model* model = new Model();
	model->setObjModel(vertexObject);
	models.push_back(model);


}

void AssimpLoader::ProcessNode(aiNode* node, const aiScene* scene, std::vector<Model*>& models, GLenum mode)
{
	
	// Process each mesh located at the current node
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->ProcessMesh(mesh, scene, models, mode);
	}
	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, models, mode);
	}
}
