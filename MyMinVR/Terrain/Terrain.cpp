#include "../Terrain/Terrain.h"
#include "../common/common.h"

#include <stb_image.h>

#include <assert.h>
#define MAX_HEIGHT 10

Terrain::Terrain(float x, float z):X(x), Z(z), heightLoaded(false)
{
	GenerateTerrain();
	if (myModel)
	{
		Texture* texture0 = new Texture(GL_TEXTURE_2D, "grass.png");  
		myModel->addTexture(texture0);

		Texture* texture1 = new Texture(GL_TEXTURE_2D, "mud.png");
		myModel->addTexture(texture1);

		Texture* texture2 = new Texture(GL_TEXTURE_2D, "rocks.png");
		myModel->addTexture(texture2);

		Texture* texture3 = new Texture(GL_TEXTURE_2D, "blendmap.png");
		myModel->addTexture(texture3);
	}
	
}

Terrain::Terrain(float x, float z, char* heightMap) :X(x), Z(z), heightLoaded(false)
{
	
    LoadHeigthMap(heightMap);
	GenerateTerrain();
	if (myModel)
	{
		Texture* texture0 = new Texture(GL_TEXTURE_2D, "../Resources/grass.png");
		myModel->addTexture(texture0);

		//Texture* texture1 = new Texture(GL_TEXTURE_2D, "Himalayas_DEM_Cube.png");
		Texture* texture1 = new Texture(GL_TEXTURE_2D, "../Resources/mud.png");
		myModel->addTexture(texture1);

		Texture* texture2 = new Texture(GL_TEXTURE_2D, "../Resources/rocks.png");
		myModel->addTexture(texture2);

		Texture* texture3 = new Texture(GL_TEXTURE_2D, "../Resources/blendmap.png");
		myModel->addTexture(texture3);
	}
	
}

Terrain::~Terrain()
{
	delete myModel;	

}

const float Terrain::GetWidthX()
{
	return X;
}

const float Terrain::GetWidthZ()
{
	return Z;
}

void Terrain::LoadHeigthMap(char* fileName)
{

	/*FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	heightLoaded = false;
	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)
	fif = FreeImage_GetFileType(fileName, 0);
	if (fif == FIF_UNKNOWN) {
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(fileName);
	}
	// check that the plugin has reading capabilities ...
	if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		// ok, let's load the file
		myHeightMap.bitmap = FreeImage_Load(fif, fileName);
		if (!myHeightMap.bitmap)
		{
			assert(false && "Image failed to load 1");
		}

		myHeightMap.width = FreeImage_GetWidth(myHeightMap.bitmap);
		myHeightMap.height = FreeImage_GetHeight(myHeightMap.bitmap);
		myHeightMap.bbp = FreeImage_GetBPP(myHeightMap.bitmap);
		// unless a bad file format, we are done !
		heightLoaded = true;
	}*/
	

	int width, height, nrChannels;
	unsigned char *data = stbi_load(fileName, &width, &height, &nrChannels, 0);

	myHeightMap.bitmap = data;
	myHeightMap.width = width;
	myHeightMap.height = height;
	myHeightMap.bbp = nrChannels;

	heightLoaded = true;

}

float Terrain::GetHeight(int x, int y)
{
	//RGBQUAD color;
	//if (FreeImage_GetPixelColor(myHeightMap.bitmap, x, y, &color))
	if (x < 0)
	{
		x = 0;
	}
	else if (x > myHeightMap.width)
	{
		x = myHeightMap.width;
	}

	if (y < 0)
	{
		y = 0;
	}
	else if (y > myHeightMap.height)
	{
		y = myHeightMap.height;
	}

	unsigned bytePerPixel = myHeightMap.bbp;
	unsigned char* pixelOffset = myHeightMap.bitmap + (x + myHeightMap.height * y) * bytePerPixel;
	unsigned char r = pixelOffset[0];
	unsigned char g = pixelOffset[1];
	unsigned char b = pixelOffset[2];
	unsigned char a = myHeightMap.bbp >= 4 ? pixelOffset[3] : 0xff;
	
	float c = MAX_HEIGHT * ((r / 255.0f) - 0.5f);
	return c;
	
	
}

void Terrain::render(ShaderProgram shaderProgram)
{
	shaderProgram.setUniformi("myGrassTextureSampler", 0); 
	shaderProgram.setUniformi("myMudTextureSampler", 1);
	shaderProgram.setUniformi("myRockTextureSampler", 2);
	shaderProgram.setUniformi("myBlendTextureSampler", 3);

	myModel->render(shaderProgram);
}

Model& Terrain::model()
{
	return *myModel;
}

void Terrain::GenerateTerrain()
{
	unsigned int VERTEX_COUNT = 256;
	int count = VERTEX_COUNT * VERTEX_COUNT;
	

	std::vector<glm::vec3> vertices(VERTEX_COUNT* VERTEX_COUNT); // early optimization??
	std::vector<glm::vec3> normals(VERTEX_COUNT* VERTEX_COUNT); // early optimization??
	std::vector<glm::vec2> uvcoords(VERTEX_COUNT* VERTEX_COUNT); // early optimization??


	std::vector<unsigned int> indx;

	//(GLfloat*)malloc(sizeof(GLfloat) * 3 * m_numvertices);
	

	float halfW = -((float)TERRAIN_SIZE / (float)2);
	float move = (float)TERRAIN_SIZE / (float)(VERTEX_COUNT - 1);

	for (int i = 0; i < VERTEX_COUNT; ++i) {

		float halfL = -((float)TERRAIN_SIZE / (float)2);
		for (int j = 0; j < VERTEX_COUNT; ++j) {

			
			float vx = ((float)halfL / ((float)VERTEX_COUNT - 1)) * TERRAIN_SIZE;
			float vy = heightLoaded ? GetHeight(j, i): 0.0f; // ;
			float vz = ((float)halfW / ((float)VERTEX_COUNT - 1))* TERRAIN_SIZE;
			glm::vec3 vertex(vx,vy,vz);

			//float vx(vx, vy, vz);
			vertices[(VERTEX_COUNT * i ) + j ] = vertex;

			//std::cout << (VERTEX_COUNT * i * 3) + j * 3 << std::endl;
			//std::cout << (VERTEX_COUNT * i * 3) + (j * 3 + 1) << std::endl;
			//std::cout << (VERTEX_COUNT * i * 3) + (j * 3 + 2) << std::endl;

			halfL += move;


			//glm::vec3  normal = heightLoaded? calculateNormal(j, i): vec3(0.0f, 1.0f, 0.0f);
			glm::vec3  normal =  vec3(0.0f, 1.0f, 0.0f);

			normals[(VERTEX_COUNT * i ) + j ] = normal;
			
			float uvx = (float)j / ((float)VERTEX_COUNT - 1);
			float uvy = (float)i / ((float)VERTEX_COUNT - 1);
			glm::vec2 uv(uvx, uvy);

			uvcoords[(VERTEX_COUNT * i ) + j ] = uv;		

		}
		halfW += move;
	}

	int pointer = 0;
	
	for (int gz = 0; gz < VERTEX_COUNT - 1; ++gz) {
		for (int gx = 0; gx < VERTEX_COUNT - 1; ++gx) {
			int topLeft = (gz*VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;

			indx.push_back(topLeft);
			indx.push_back( bottomLeft );
			indx.push_back(topRight );
			/*indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);*/

			//std::cout << (VERTEX_COUNT * gz * 6)  + gx * 6  << std::endl;
			//std::cout << (VERTEX_COUNT * gz * 6)  + (gx * 6+ 1) << std::endl;
			//std::cout << (VERTEX_COUNT * gz * 6)  + (gx * 6 + 2) << std::endl;

			indx.push_back(topRight);
			indx.push_back(bottomLeft);
			indx.push_back(bottomRight);
			/*indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);*/

			//std::cout << (VERTEX_COUNT * gz * 6) + (gx * 6 + 3) << std::endl;
			//std::cout << (VERTEX_COUNT * gz * 6) + (gx * 6 + 4) << std::endl;
			//std::cout << (VERTEX_COUNT * gz * 6) + (gx * 6 + 5) << std::endl;

		}
	}

	VertexBuffer* vertexObject = new VertexBuffer();
	vertexObject->setData(  vertices, normals, uvcoords, indx);
	
	myModel = new Model();
	if (myModel)
	{
		myModel->setObjModel(vertexObject);
	}
	

}

glm::vec3 Terrain::calculateNormal(int x, int y)
{
	float heightL = GetHeight(x - 1, y);
	float heightR = GetHeight(x + 1, y);
	float heightD = GetHeight(x, y - 1);
	float heightU = GetHeight(x, y + 1);
	vec3 normal(heightL - heightR, 2.0f, heightD - heightU);
	normal = normalize(normal);
	return normal;
}

