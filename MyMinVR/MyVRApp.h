#ifndef MYVRAPP_H
#define MYVRAPP_H

//#include <osgViewer/Viewer>

#include <api/MinVR.h>
#include "ShaderProgram/ShaderProgram.h"

#include <VRMenuHandler.h>
#include <imgui/imfilebrowser.h>

// forward declaration
class Texture;
class Model;
class Terrain;
class Camera;
class MinVR::VRDataIndex;

class MyVRApp : public MinVR::VRApp
{
	

public:

	enum Mode
	{
		vr,
		cave,
		desktop
	};

	MyVRApp(int argc, char *argv[]);

	virtual ~MyVRApp();

	/**
	* Called whenever an new input event happens.
	*/
	/*void onVREvent(const MinVR::VRDataIndex &eventData);*/

	/**
	* Called before renders to allow the user to set context-specific variables.
	*/
	void onRenderGraphicsContext(const MinVR::VRGraphicsState &renderState);

	/**
	* Called when the application draws.
	*/
	void onRenderGraphicsScene(const MinVR::VRGraphicsState &renderState);



protected:

	void initGraphicsDrivers();

	void initShaders();
	
	void initModel();

	void renderScene(const MinVR::VRGraphicsState &renderState);

	void initCamera(int width, int height);

	void rotateOnYaxis(Model& object, float direction);

	void onAnalogChange(const MinVR::VRAnalogEvent &event);

	void onButtonDown(const MinVR::VRButtonEvent &event);

	void onButtonUp(const MinVR::VRButtonEvent &event);

	void onCursorMove(const MinVR::VRCursorEvent &event);

	void onTrackerMove(const MinVR::VRTrackerEvent &event);

protected:

  
	void renderRay(const glm::vec3& from, const glm::quat& direction);
	void rayInterection();

	void renderSphere(const MinVR::VRGraphicsState &renderState);
	void renderListOfMeshes(const MinVR::VRGraphicsState &renderState);

  void menu_callback();
  void menu_callback2();

  void loadFileModel(std::string& fileName);
  Model* TestRayEntityIntersection(vec3 ray);

  glm::vec3 RayCast(float mouse_x, float mouse_y);

	glm::mat4 m_terrain;

	GLuint m_vbo;
	GLuint m_vao;

	GLuint m_vs;
	GLuint m_fs;
	GLuint m_shader;
	
	Mode myMode;
	std::string myFilePath;

	Model* objModel;
  Model* objModel2;

	Model* x3dModel;
	Model* skyDomeModel;
	Model* sphereModel;
	std::vector<Model*> loadedModels;

	Terrain* terrain;

	//osg::ref_ptr<osgViewer::Viewer> viewer;

	ShaderProgram simpleTextureShader;
	ShaderProgram simpleColorShader;
	ShaderProgram terrainShader;
	ShaderProgram atmosphereShader;
	ShaderProgram LinesShader;

	glm::vec3 lastControllerPosition;
	glm::quat controllerOrientation;
  glm::mat4 controllerTransform;
  bool grab;

	// shader input
	float fInnerRadius = 10.0f;
	float fOuterRadius = 60.25f;

	Camera* camera;
  glm::vec2 cursorCurrentPos;
  glm::vec2 lastMousePosition;



  Model* mouseSelectedEntity;
  

	GLuint bVao;
	GLuint bVbo;
	bool showRayByRotation;
	bool showRayByTranslation;
	glm::vec3 vrRay;
	bool rotateObject;
	bool translateObject;

  VRMenuHandler *menus;
  ImGui::FileBrowser fileDialog;

	glm::vec3 carpetPosition, carpetUp, carpetScale;
	float carpetDirection; // (expressed in radians)
	glm::quat carpetQuatDirection;
  bool m_is2d;
};

#endif