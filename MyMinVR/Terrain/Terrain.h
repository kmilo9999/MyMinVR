#ifndef TERRAIN_H
#define TERRAIN_H

#include "../Model/Model.h"
//#include <FreeImage\FreeImage.h>


#define TERRAIN_SIZE 150
#define MAX_HEIGH 40

class Terrain
{
	struct HeightMap
	{
		int height = 0;
		int width = 0;
		unsigned char* bitmap;
		unsigned int bbp = 0;
	};

		
public:

	Terrain(float x, float z);
	Terrain(float x, float z, char* heightMap);
	~Terrain();

	const float GetWidthX();
	const float GetWidthZ();

	

	float GetHeight(int x, int y);

	void render(ShaderProgram shader);

	Model& model();

private:

	float X;
	float Z;
	HeightMap myHeightMap;

	void  LoadHeigthMap(char* heightMap);
	void GenerateTerrain();
	vec3 calculateNormal(int x, int y);

	bool heightLoaded;

	Model* myModel;

};

#endif