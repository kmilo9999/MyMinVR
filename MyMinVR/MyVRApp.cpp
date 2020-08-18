#include <fstream>
#include <sstream>

//
//#include <osg/Geode>
//#include <osg/Geometry>
//#include <osg/Material>
//#include <osg/Vec3>
//#include <osg/MatrixTransform>
//#include <osg/Texture2D>
//#include <osg/PolygonStipple>
//#include <osg/TriangleFunctor>
//#include <osg/io_utils>
//#include <osg/ShapeDrawable>
//
//#include <osgDB/ReadFile>
//#include <osgDB/WriteFile>
//
//#include <osgGA/TrackballManipulator>
//
//
//
//#include <osg/Math>

#include <iostream>
#include "MyVRApp.h"
#include "Texture/Texture.h"
#include "GLMLoader/GLMLoader.h"
#include "Terrain/Terrain.h"
#include "Camera/Camera.h"
#include "Model/Model.h"
#include <math.h>
#include <string>
# define M_PI           3.14159265358979323846  /* pi */

#include "AssimpLoader/AssimpLoader.h"
#include <glm/gtx/matrix_decompose.hpp>

MyVRApp::MyVRApp(int argc, char *argv[])
:m_vbo(0),
m_vao(0),
m_vs(0),
m_fs(0),
m_shader(0),
showRayByRotation(false),
showRayByTranslation(false),
rotateObject(false), translateObject(false),
camera(0),
menus(0),
//objModel(0),
m_is2d(true),
mouseSelectedEntity(0),
grab(false),
VRApp(argc, argv)
{ 
  
	//MinVR::VRMain::registerEventHandler(this);
	{
		for (int index = 1; index < argc; ++index)
		{

			if (strcmp(argv[index], "-c") == 0)
			{
				std::string fileConf = argv[index + 1];
				if (fileConf.find("HTC_Vive.minvr") != std::string::npos)
				{
					myMode = vr;
          m_is2d = false;
				}
        else if (fileConf.find("RemoteDisplay.minvr") != std::string::npos)
        {
          myMode = vr;
        }
				else if (fileConf.find("yurt.minvr") != std::string::npos)
				{
					myMode = cave;
          m_is2d = false;
				}
				else
				{
					myMode = desktop;
				}
			}

			if (strcmp(argv[index], "-f") == 0)
			{
				if (argv[index + 1])
				{
					myFilePath = argv[index + 1];
				}
				break;
			}
      
      menus = new VRMenuHandler(m_is2d);
      menus->addNewMenu(std::bind(&MyVRApp::menu_callback, this), 1024, 1024, 1, 1);
      VRMenu* menu = menus->addNewMenu(std::bind(&MyVRApp::menu_callback2, this), 1024, 1024, 1, 1);
      menu->setMenuPose(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 2, 2, 0, 1));
		}


	}

}

MyVRApp::~MyVRApp() 
{
	
	delete camera;
	//delete objModel;
	delete skyDomeModel;
	delete terrain;
	delete x3dModel;
  for (int i = 0 ;  i < loadedModels.size(); i++)
  {
    delete loadedModels[i];
  }
}


void MyVRApp::initGraphicsDrivers()
{
	// Initialize GLEW.
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Error initializing GLEW." << std::endl;
		return;
	}

	// Initialize OpenGL.
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.3f, 0.2, 1.0f);
}

void MyVRApp::initShaders()
{
	// Create the shader and links it 
	simpleTextureShader.LoadShaders("Shaders/shader.vert", "Shaders/shader.frag");
	simpleTextureShader.addUniform("m");
	simpleTextureShader.addUniform("v");
	simpleTextureShader.addUniform("p");
  simpleTextureShader.addUniform("intersected");
  simpleTextureShader.addUniform("color");

	simpleColorShader.LoadShaders("Shaders/simpleColorShader.vert", "Shaders/simpleColorShader.frag");
	simpleColorShader.addUniform("m");
	simpleColorShader.addUniform("v");
	simpleColorShader.addUniform("p");

	terrainShader.LoadShaders("Shaders/terrainShader.vert", "Shaders/terrainShader.frag");
	terrainShader.addUniform("m");
	terrainShader.addUniform("v");
	terrainShader.addUniform("p");
	terrainShader.addUniform("myGrassTextureSampler");
	terrainShader.addUniform("myMudTextureSampler");
	terrainShader.addUniform("myRockTextureSampler");
	terrainShader.addUniform("myBlendTextureSampler");



	atmosphereShader.LoadShaders("Shaders/atmosphere.vert", "Shaders/atmosphere.frag");
	atmosphereShader.addUniform("m");
	atmosphereShader.addUniform("v");
	atmosphereShader.addUniform("p");
	atmosphereShader.addUniform("v3CameraPos");
	atmosphereShader.addUniform("v3LightPos");
	atmosphereShader.addUniform("v3InvWavelength");
	atmosphereShader.addUniform("fCameraHeight");
	atmosphereShader.addUniform("fCameraHeight2");
	atmosphereShader.addUniform("fInnerRadius");
	atmosphereShader.addUniform("fInnerRadius2");
	atmosphereShader.addUniform("fOuterRadius");
	atmosphereShader.addUniform("fOuterRadius2");
	atmosphereShader.addUniform("fKrESun");
	atmosphereShader.addUniform("fKmESun");
	atmosphereShader.addUniform("fKr4PI");
	atmosphereShader.addUniform("fKm4PI");
	atmosphereShader.addUniform("fScale");
	atmosphereShader.addUniform("fScaleDepth");
	atmosphereShader.addUniform("fScaleOverScaleDepth");
	atmosphereShader.addUniform("fSamples");
	atmosphereShader.addUniform("nSamples");


	LinesShader.LoadShaders("Shaders/linesShader.vert", "Shaders/linesShader.frag");
	LinesShader.addUniform("m");
	LinesShader.addUniform("v");
	LinesShader.addUniform("p");

}

void MyVRApp::initModel()
{


	//std::string x3dFileName = "myX3d2.x3d";
	//x3dModel = AILoader::GetInstance()->loadFile(x3dFileName);
	//x3dModel->setPosition(glm::vec3(0.0f, 0.0f, -5.0f));

	// init buffer for ray
	glGenVertexArrays(1, &bVao);
	glGenBuffers(1, &bVbo);

	std::string objFileName = "../Resources/cube.obj";
  Model* objModel = GLMLoader::loadModel(objFileName);

  //std::string objFileName = "../Resources/cube.obj";
  Model* objModel2 = GLMLoader::loadModel(objFileName);

	//// Initialize texture
	Texture* m_texture = new Texture(GL_TEXTURE_2D, "../Resources/Himalayas_DEM_Cube.jpg");
	objModel->addTexture(m_texture);
	objModel->tranform().setPosition(glm::vec3(0, 0, -5.0f));
	objModel->tranform().setScale(glm::vec3(0.25f, 0.25f, 0.25f));
  objModel->setColor(glm::vec4(0.9f, 0.0f, 0.0f, 1.0f));

  objModel2->addTexture(m_texture);
  objModel2->tranform().setPosition(glm::vec3(0, -2, -5.0f));
  objModel2->tranform().setScale(glm::vec3(0.25f, 0.25f, 0.25f));
  objModel2->setColor(glm::vec4(0.9f, 0.9f, 0.0f, 1.0f));
  //objModel->addChild(objModel2);

	loadedModels.push_back(objModel);
  loadedModels.push_back(objModel2);


	glm::vec3 rotAxis(0, 0, 1);
	quat q(
		cosf(radians(-90.0f) / 2.f),
		rotAxis.x * sinf(radians(-90.0f) / 2.f),
		rotAxis.y * sinf(radians(-90.0f) / 2.f),
		rotAxis.z * sinf(radians(-90.0f) / 2.f));

	//objModel->setOrientation(q);


	//std::string meshesFile = "../Resources/buildingYunny2.obj";
	//loadedModels = AssimpLoader::GetInstance()->loadFile(meshesFile);
	//sphereModel->setPosition(glm::vec3(0, 0, 0));
	//sphereModel->setScale(glm::vec3(0.5, 0.5,0.5));


	// SkyDome
	std::string skyDomeFileName = "../Resources/dome.obj";
	skyDomeModel = GLMLoader::loadModel(skyDomeFileName);
	Texture* skyTexture = new Texture(GL_TEXTURE_2D, "../Resources/SkyDome.png");
	skyDomeModel->addTexture(skyTexture);
        glm::vec3 skyInitPos = glm::vec3(0.0f, -10.0f, -5.0f);
        glm::vec3 skyInitScale = glm::vec3(70.0f, 70.0f, 70.0f);
	skyDomeModel->tranform().setPosition(skyInitPos);
	skyDomeModel->tranform().setScale(skyInitScale);


	terrain = new Terrain(80, 80,"../Resources/mceclip6.png");
	
        glm::vec3 tererainInitPos = glm::vec3(-2.0f, -2.0f, -2.0f);
	terrain->model().tranform().setPosition(tererainInitPos);
    
	

}

void MyVRApp::renderScene(const MinVR::VRGraphicsState &renderState)
{

	const float* vm = renderState.getViewMatrix();
	
  
  glDisable(GL_CULL_FACE);
	if (skyDomeModel)
  //if (false)
	{
		glFrontFace(GL_CW);
		glBlendFunc(GL_ONE, GL_ONE);
		atmosphereShader.start();
		glm::vec3 wl(powf(0.650f, 4.0), powf(0.570f, 4.0), powf(0.475f, 4.0));
		glm::vec3 cameraHeightV = camera->myPosition - terrain->model().tranform().position();
		float cameraHeight = glm::length(cameraHeightV);
		//float fInnerRadius  = 10.0f;
		//float fOuterRadius  = 60.25f;
		float fScale = 1.0f / (fOuterRadius - fInnerRadius);
		glm::vec3 v3InvWavelength = glm::vec3(1.0 / wl.x, 1.0 / wl.y, 1.0 / wl.z);
		float fKr4PI = 0.025f*4.0f*M_PI;
		float fKm4PI = 0.015f*4.0f*M_PI;
		float fScaleOverScaleDepth = ((1.0f / (fOuterRadius - fInnerRadius)) / 0.09);
		//std::cout << v3InvWavelength.x * fKr4PI + fKm4PI << " "<< v3InvWavelength.y * fKr4PI + fKm4PI << " " <<v3InvWavelength.z * fKr4PI + fKm4PI << "\n";
		atmosphereShader.setUniformMatrix4fv("p", renderState.getProjectionMatrix());

		if (myMode == desktop)
		{
			atmosphereShader.setUniform("v", camera->GetView());
		}
		else
		{
			atmosphereShader.setUniformMatrix4fv("v", vm);
		}
		


		
		atmosphereShader.setUniform("v3CameraPos", camera->myPosition);
		atmosphereShader.setUniform("v3LightPos", glm::vec3(0.0, 100.0, -10.0));
		atmosphereShader.setUniform("v3InvWavelength", v3InvWavelength);
		atmosphereShader.setUniformf("fCameraHeight", cameraHeight);
		atmosphereShader.setUniformf("fCameraHeight2",cameraHeight * cameraHeight);
		atmosphereShader.setUniformf("fInnerRadius",fInnerRadius);
		atmosphereShader.setUniformf("fInnerRadius2",fInnerRadius * fInnerRadius);
		atmosphereShader.setUniformf("fOuterRadius",fOuterRadius);
		atmosphereShader.setUniformf("fOuterRadius2",fOuterRadius * fOuterRadius);
		atmosphereShader.setUniformf("fKrESun",0.0025f * 15.0f);
		atmosphereShader.setUniformf("fKmESun",0.0015f * 15.0f);
		atmosphereShader.setUniformf("fKr4PI", fKr4PI);
		atmosphereShader.setUniformf("fKm4PI", fKm4PI);
		atmosphereShader.setUniformf("fScale", fScale);
		atmosphereShader.setUniformf("fScaleDepth",0.25f);
		atmosphereShader.setUniformf("fScaleOverScaleDepth", fScaleOverScaleDepth);
		atmosphereShader.setUniformf("fSamples", 2.0f);
		atmosphereShader.setUniformi("nSamples", 2);
		skyDomeModel->render(atmosphereShader);
		atmosphereShader.stop();
		glBlendFunc(GL_ONE, GL_ZERO);
		glFrontFace(GL_CCW);

	
//		/*const float* vm = renderState.getViewMatrix();
//		glm::mat4 viewMatrix = glm::mat4(vm[0], vm[1], vm[2], vm[3],
//			vm[4], vm[5], vm[6], vm[7],
//			vm[8], vm[9], vm[10], vm[11],
//			vm[12], vm[13], vm[14], vm[15]);
//*/
//		simpleShader.setUniform("v", camera->GetView());
//		//objModel->render(simpleShader);
//        skyDomeModel->render(simpleShader);
//		simpleShader.stop();
		
	}


  glEnable(GL_CULL_FACE);
	//if(x3dModel)
	if (false)
	{
		simpleColorShader.start();
		simpleColorShader.setUniformMatrix4fv("p", renderState.getProjectionMatrix());
		simpleColorShader.setUniform("v", camera->GetView());
		//simpleColorShader.setUniformMatrix4fv("v", vm);
		x3dModel->render(simpleColorShader);
		simpleColorShader.stop();
	}

	renderListOfMeshes(renderState);

	//renderSphere(renderState);

	//if (objModel)
	//if (false)
	//{
	//	simpleTextureShader.start();
	//	simpleTextureShader.setUniformMatrix4fv("p", renderState.getProjectionMatrix());
	//	if (myMode == desktop)
	//	{
	//		simpleTextureShader.setUniform("v", camera->GetView());
	//	}
	//	else
	//	{
	//		simpleTextureShader.setUniformMatrix4fv("v", vm);
	//	}
 //   
 //   simpleTextureShader.setUniform("color", objModel->color());

 //   if (objModel->isSelected())
 //   {
 //     glEnable(GL_BLEND);
 //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 //     simpleTextureShader.setUniformi("intersected", 1);
 //     objModel->render(simpleTextureShader);
 //     glEnable(GL_BLEND);
 //   }
 //   else
 //   {
 //     simpleTextureShader.setUniformi("intersected", 0);
 //     objModel->render(simpleTextureShader);
 //   }
 //   
 //   simpleTextureShader.setUniform("color", objModel2->color());
 //   
 //   //simpleTextureShader.setUniform("color", color);

 //   if (objModel2->isSelected())
 //   {
 //     glEnable(GL_BLEND);
 //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 //     simpleTextureShader.setUniformi("intersected", 1);
 //     objModel2->render(simpleTextureShader);
 //     glEnable(GL_BLEND);
 //   }
 //   else
 //   {
 //     simpleTextureShader.setUniformi("intersected", 0);
 //     objModel2->render(simpleTextureShader);
 //   }

 //   
 //   

	//	simpleTextureShader.stop();
	//}


	if (terrain)
	//if (false)
	{
		terrainShader.start();

		terrainShader.setUniformMatrix4fv("p", renderState.getProjectionMatrix());
		if (myMode == desktop)
		{
			terrainShader.setUniform("v", camera->GetView());
		}
		else
		{
			terrainShader.setUniformMatrix4fv("v", vm);
		}
		
		
		terrain->render(terrainShader);
		// Reset the shader.
		terrainShader.stop();
	}

	//if (showRayByRotation || showRayByTranslation)
  if(true)
	{
		


		LinesShader.start();
		LinesShader.setUniformMatrix4fv("p", renderState.getProjectionMatrix());
		if (myMode == desktop)
		{
			LinesShader.setUniform("v", camera->GetView());
		}
		else
		{
			LinesShader.setUniformMatrix4fv("v", vm);
		}

		
		
		const float* camPos = renderState.getCameraPos();
		glm::vec3 cPos(camPos[0], camPos[1], camPos[2]);

    /*std::cout << cPos.x << ", " << cPos.y << ", " <<
      cPos.z << std::endl;*/

    glm::vec3 controllerPosition;
    glm::quat controllerOrientation;
    glm::vec3 controllerScale;
    

    glm::decompose(controllerTransform, controllerScale,
      controllerOrientation, controllerPosition, glm::vec3(), glm::vec4());
   

		glm::mat4 ModelMatrix = glm::mat4(1);
    ModelMatrix *= glm::translate(ModelMatrix, controllerPosition);
		glm::mat4 rot =  glm::toMat4(glm::quat());
		glm::mat4 scal = glm::scale(vec3(1));

		LinesShader.setUniform("m", ModelMatrix*rot*scal);

    
		renderRay(glm::vec3(0,0,0), controllerOrientation);
		LinesShader.stop();
		
	}
}

void MyVRApp::initCamera(int width, int height)
{
	camera = new Camera(width, height, glm::vec3(0, 1, 8), glm::vec3(0,0,-1));
  
}

void MyVRApp::rotateOnYaxis(Model& object, float direction)
{


	glm::quat currentQ = object.tranform().orientation();

	glm::vec3 rotAxis(0, 1, 0);
	glm::quat q(
		cosf(radians(direction) / 2.0f),
		rotAxis.x * sinf(radians(direction) / 2.0f),
		rotAxis.y * sinf(radians(direction) / 2.0f),
		rotAxis.z * sinf(radians(direction) / 2.0f));

	object.tranform().setOrientation(q * currentQ);
}

void MyVRApp::onAnalogChange(const MinVR::VRAnalogEvent &event)
{
	//std::cout << event.getName() << "\n";
	//HTC_Controller_Right_Trigger1
	//HTC_Controller_Right_Trigger2
	if (myMode != desktop)
	{
		if (myMode == vr)
		{
			if (event.getName().find("HTC_Controller_1_") != -1
				|| event.getName().find("HTC_Controller_Right_") != -1)
			{
				//if (event.getName().find("Joystick") != -1
				//	&& objModel
				//	)
				//{
				//	//int pos1 = event.getName().find("Joystick");
				//	//std::string axisName = event.getName().substr(pos1 + 8);
				//	float direction = event.getValue();
				//	//std::cout << axisName << " " << direction << "\n";
				//	rotateOnYaxis(*objModel, direction);
				//}

				if (event.getName().find("Trigger") != -1)
				{
					/*int pos1 = event.getName().find("Trigger");
					std::string axisName = event.getName().substr(pos1 + 6);
					std::cout << axisName << "\n";*/
          if (event.getValue() < 0.3)
          {
            grab = false;
          }
          else
          {
            grab = true;
          }
          
				}
				/*std::list<std::string> names = event.index().findAllNames();
				for (string name:names)
				{
					std::cout << name << "\n";

				}*/
			}

      if (menus != NULL && menus->windowIsActive() 
        && event.getName() == "HTC_Controller_Right_TrackPad0_Y" 
        || event.getName() == "HTC_Controller_1_TrackPad0_Y")
      {
        menus->setAnalogValue(event.getValue());
      }

			if (event.getName().find("HTC_Controller_1_Trigger1") != -1
				|| event.getName().find("HTC_Controller_Right_Trigger1") != -1)
			{
				/*if (event.getValue() < 0.3)
				{
					showRayByRotation = false;

					return;
				}
        */
				glm::quat q = controllerOrientation;
				vrRay.x = 2 * q.x*q.z - 2 * q.y*q.w;
				vrRay.y = 2 * q.y*q.z + 2 * q.x*q.w;
				vrRay.z = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
				vrRay = glm::normalize(vrRay);

        /*
				showRayByRotation = true;

				return;*/

        

			}

			if (event.getName().find("HTC_Controller_1_Trigger2") != -1
				|| event.getName().find("HTC_Controller_Right_Trigger2") != -1)
			{
				if (event.getValue() < 0.3)
				{
					showRayByTranslation = false;
					//translateObject = false;
					return;
				}

				glm::quat q = controllerOrientation;
				vrRay.x = 2 * q.x*q.z - 2 * q.y*q.w;
				vrRay.y = 2 * q.y*q.z + 2 * q.x*q.w;
				vrRay.z = 1 - 2 * q.x * q.x - 2 * q.y * q.y;
				vrRay = glm::normalize(vrRay);

				//translateObject = true;
				showRayByTranslation = true;

				return;
			}

		}
		else if (myMode == cave)
		{
			//if (event.getName() == "Wand_Joystick_Y_Update"
			//	&& !(event.getValue() > -0.1 && event.getValue() < 0.1))
			//{
			//	//int pos1 = event.getName().find("Joystick");
			//	//std::string axisName = event.getName().substr(pos1 + 8);
			//	float direction = event.getValue();
			//	//std::cout << axisName << " " << direction << "\n";
			//	rotateOnYaxis(*objModel, direction);
			//}
				
		}


	}
	
		
}

void MyVRApp::onButtonDown(const MinVR::VRButtonEvent &event)
{
  if (menus != NULL && menus->windowIsActive()) {
    if (event.getName() == "HTC_Controller_Right_Axis1Button_Down" 
      || event.getName() == "HTC_Controller_1_Axis1Button_Down")
    {
      //left click
      menus->setButtonClick(0, 1);
    }
    else if (event.getName() == "HTC_Controller_Right_GripButton_Down" 
      || event.getName() == "HTC_Controller_1_GripButton_Down")
    {
      //middle click
      menus->setButtonClick(2, 1);
    }
    //else if (event.getName() == "HTC_Controller_Right_AButton_Down" || event.getName() == "HTC_Controller_1_AButton_Down")
    else if (event.getName() == "HTC_Controller_Right_Axis0Button_Down" 
      || event.getName() == "HTC_Controller_1_Axis0Button_Down")
    {
      //right click
      menus->setButtonClick(1, 1);
    }
    else if (event.getName() == "MouseBtnLeft_Down")
    {
      menus->setButtonClick(0, 1);
    }
    else if (event.getName() == "MouseBtnRight_Down")
    {
      menus->setButtonClick(1, 1);
    }
  }


	if (event.getName() == "MouseBtnRight_Down")
	{
		if (camera)
		{
			camera->SetCaRotate(true);
		}
	}
  
  if (event.getName() == "MouseBtnLeft_Down")
  {
    
    glm::vec3 ray = RayCast(cursorCurrentPos.x, cursorCurrentPos.y);
    //std::cout << ray.x << "," << ray.y << "," << ray.z << std::endl;
    mouseSelectedEntity = TestRayEntityIntersection(ray);
    if (mouseSelectedEntity)
    {
      std::cout << "PICK SOMETHING" << std::endl;

    }
    else
    {
      std::cout << "NO PICK SOMETHING" << std::endl;
    }
  }



	if (event.getName() == "KbdW_Down")
	{
		if (camera)
		{
			camera->Move(Camera::forward);
		}

	}

	if (event.getName() == "KbdS_Down")
	{
		if (camera)
		{
			camera->Move(Camera::backward);
		}
	}

	if (event.getName() == "KbdA_Down")
	{
		if (camera)
		{
			camera->Move(Camera::left);
		}
	}

	if (event.getName() == "KbdD_Down")
	{
		if (camera)
		{
			camera->Move(Camera::right);
		}
	}

	if (event.getName() == "KbdQ_Down")
	{
		if (camera)
		{
			camera->Move(Camera::Up);
		}
	}

	if (event.getName() == "KbdE_Down")
	{
		if (camera)
		{
			camera->Move(Camera::Down);
		}
	}

	// Press escape to quit.
	if (event.getName() == "KbdEsc_Down") {
		shutdown();
	}
}

void MyVRApp::onButtonUp(const MinVR::VRButtonEvent &event)
{

  if (menus != NULL)
  {
    if (event.getName() == "HTC_Controller_Right_Axis1Button_Up" || event.getName() == "HTC_Controller_1_Axis1Button_Up")
    {
      //left click
      menus->setButtonClick(0, 0);

    }
    else if (event.getName() == "HTC_Controller_Right_GripButton_Up" || event.getName() == "HTC_Controller_1_GripButton_Up")
    {
      //middle click
      menus->setButtonClick(2, 0);
    }
    //else if (event.getName() == "HTC_Controller_Right_AButton_Down" || event.getName() == "HTC_Controller_1_AButton_Down")
    else if (event.getName() == "HTC_Controller_Right_Axis0Button_Up" || event.getName() == "HTC_Controller_1_Axis0Button_Up")
    {
      //right click
      menus->setButtonClick(1, 0);
    }
    if (event.getName() == "MouseBtnLeft_Up")
    {
      menus->setButtonClick(0, 0);
    }
    else if (event.getName() == "MouseBtnRight_Up")
    {
      menus->setButtonClick(1, 0);
    }
    else if (menus->windowIsActive() && event.getName() == "MouseBtnMiddle_ScrollUp")
    {
      menus->setAnalogValue(10);
    }

    if (menus->windowIsActive() && event.getName() == "MouseBtnMiddle_ScrollDown")
    {
      menus->setAnalogValue(-10);
    }

  }

	if (myMode == desktop)
	{
    if (event.getName() == "MouseBtnLeft_Up")
    {
      if (mouseSelectedEntity)
      {
        mouseSelectedEntity = 0;
      }
      
    }

		if (event.getName() == "MouseBtnRight_Up")
		{
			if (camera)
			{
				camera->SetCaRotate(false);
			}
		}


		if (event.getName() == "KbdW_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::forward);
			}
		}

		if (event.getName() == "KbdS_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::backward);
			}

		}

		if (event.getName() == "KbdA_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::left);
			}
		}

		if (event.getName() == "KbdD_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::right);
			}
		}


		if (event.getName() == "KbdQ_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::Up);
			}
		}



		if (event.getName() == "KbdE_Up")
		{
			if (camera)
			{
				camera->Stop(Camera::Down);
			}
		}



	}
		
}

void MyVRApp::onCursorMove(const MinVR::VRCursorEvent &event)
{
	if (myMode == desktop)
	{
    
		if (event.getName() == "Mouse_Move")
		{
      menus->setCursorPos(event.getPos()[0], event.getPos()[1]);  
			//std::cout << event.getName() << std::endl;
			const float* pos = event.getPos();
			glm::vec2 npos(pos[0], pos[1]);
      cursorCurrentPos = npos;
      //std::cout << "cursor pos" <<cursorCurrentPos.x << " " << cursorCurrentPos.y << std::endl;
			if (camera)
			{
				camera->RotateView(npos);
			}

      if (!mouseSelectedEntity)
      {
        lastMousePosition.x = cursorCurrentPos.x;
        lastMousePosition.y = cursorCurrentPos.y;
        
      }
      else
      {
        vec3 rightVector = glm::normalize(glm::cross(camera->myFront, camera->myUp))*0.06f;
        vec3 upvector = glm::normalize(camera->myUp)*0.06f;
        float deltaX = (cursorCurrentPos.x - lastMousePosition.x);
        float deltaY = (cursorCurrentPos.y - lastMousePosition.y);
        float absolutX = abs(deltaX);
        float absolutY = abs(deltaY);
        vec3 newPosition = mouseSelectedEntity->tranform().position();
        if (deltaX && deltaX < 0 && absolutX > 2.5)
        {
          newPosition -= rightVector;
        }
        else if (deltaX && deltaX > 0 && absolutX > 2.5)
        {
          newPosition += rightVector;
        }

        if (deltaY && deltaY < 0 && absolutY > 2.5)
        {
          newPosition += upvector;
        }
        else if (deltaY && deltaY > 0 && absolutY > 2.5)
        {
          newPosition -= upvector;
        }
        mouseSelectedEntity->tranform().setPosition(newPosition);
        lastMousePosition.x = cursorCurrentPos.x;
        lastMousePosition.y = cursorCurrentPos.y;
      }

		}
	}
	
}

void MyVRApp::onTrackerMove(const MinVR::VRTrackerEvent &event)
{
	
  if (event.getName() == "HTC_Controller_Right_Move" 
    || event.getName() == "HTC_Controller_1_Move") {
    menus->setControllerPose(glm::make_mat4(event.getTransform()));
  }
  
  //std::cout << event.getName() << "\n";
	if (//event.getName() == "HTC_Controller_1_Move" ||
		 event.getName() == "HTC_Controller_2_Move" ||
		 //event.getName() == "HTC_Controller_Left_Move" ||
		 event.getName() == "HTC_Controller_Right_Move")
	{
    //std::cout << "here 1" << std::endl;
		glm::mat4 newPose = glm::make_mat4(event.getTransform());
    
    //if (objModel && grab)
    std::string res = grab ? "true" : "false";
    std::cout << "grab " << res << std::endl;

    if (mouseSelectedEntity && grab)
    {

      mouseSelectedEntity->tranform().applyMatrix(newPose * glm::inverse(controllerTransform));
    }
    
    if (false)
    {

      glm::vec3 trControllerPosition;
      glm::quat trControllerOrientation;
      glm::vec3 trControllerScale;

      glm::decompose(controllerTransform, trControllerScale,
        trControllerOrientation, trControllerPosition, glm::vec3(), glm::vec4());

     // objModel2->tranform().setPosition(trControllerPosition);
      //objModel2->tranform().applyMatrix(newPose );
    }

    controllerTransform = newPose;

    rayInterection();

    //controllerTransform  = transpose(controllerTransform);

    
    //controllerTransform = glm::transpose(controllerTransform);
    /*for (int i = 0; i < 4 ; i ++)
    {
      std::cout << "[";
      for (int j = 0; j < 4; j++)
      {
        std::cout << transformM[i * 4 + j] << " ";
      }
      std::cout << "]" << std::endl;
    }
    std::cout <<  std::endl;*/

		glm::quat q;
		q.w = sqrt(fmax(0, 1 + controllerTransform[0][0] + controllerTransform[1][1] + controllerTransform[2][2])) / 2;
		q.x = sqrt(fmax(0, 1 + controllerTransform[0][0] - controllerTransform[1][1] - controllerTransform[2][2])) / 2;
		q.y = sqrt(fmax(0, 1 - controllerTransform[0][0] + controllerTransform[1][1] - controllerTransform[2][2])) / 2;
		q.z = sqrt(fmax(0, 1 - controllerTransform[0][0] - controllerTransform[1][1] + controllerTransform[2][2])) / 2;
		q.x = copysign(q.x, controllerTransform[2][1] - controllerTransform[1][2]);
		q.y = copysign(q.y, controllerTransform[0][2] - controllerTransform[2][0]);
		q.z = copysign(q.z, controllerTransform[1][0] - controllerTransform[0][1]);
    controllerOrientation = q;
    //std::cout << q.x << " " << q.y << " " << q.y << " " << q.w << std::endl;

		/*if (objModel && rotateObject)
		{

			glm::quat qdelta = q * glm::inverse(controllerOrientation);

			glm::quat currentQ = objModel->orientation();
			
			objModel->setOrientation(qdelta * currentQ);

			controllerOrientation = q;
			
		
			
		}
		else
		{
			controllerOrientation = q;
			
		}*/

    
		/*const float* pos = event.getPos();

		glm::vec3 npos(pos[0], pos[1], pos[2]);
		if (objModel && translateObject)
		{
			glm::vec3 deltaPos = (npos - lastControllerPosition);

			glm::vec3 currenP = objModel->position();

			currenP += deltaPos *5.0f;
			objModel->setPosition(currenP);
			lastControllerPosition = npos;
		}
		else
		{
			lastControllerPosition = npos;
		}
    */

        //std::cout << npos.x << " " << npos.y << "" << npos.z <<"\n";
		
	}
}

void MyVRApp::renderRay(const glm::vec3& from,const glm::quat& direction)
{
  
  
	std::vector<glm::vec3> rayVector;
	//rayVector.clear();
  glm::vec3 forward = direction * glm::vec3(0, 0, -1);
	rayVector.push_back(from);
	glm::vec3 to = from + (20.0f * forward);
	rayVector.push_back(to);

  /*std::cout << "ray from: " << rayVector[0].x
    <<","<< rayVector[0].y << " ,"<< rayVector[0].z
    << " to : " << rayVector[1].x << " ," << rayVector[1].y 
    << " ," << rayVector[1].z <<std::endl;*/
	
	glBindVertexArray(bVao);
	glBindBuffer(GL_ARRAY_BUFFER, bVbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * rayVector.size(),
		&rayVector[0],
		GL_STATIC_DRAW
	);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(bVao);
	glDrawArrays(GL_LINES, 0, rayVector.size());
	//std::cout << "draw line " << std::endl;
	glBindVertexArray(0);
}

void MyVRApp::rayInterection()
{
	//  

  glm::vec3 controllerPosition;
  glm::quat controllerOrientation;
  glm::vec3 controllerScale;


  glm::decompose(controllerTransform, controllerScale,
    controllerOrientation, controllerPosition, glm::vec3(), glm::vec4());

	glm::vec3 start = controllerPosition;
  for (Model* model : loadedModels)
  {
    if (model->RayInstersection(start, vrRay))
    {
      mouseSelectedEntity = model;
      return;
    }
  }
  
  /*if (objModel->RayInstersection(start, vrRay))
  {
    
    
  }
  if (objModel2->RayInstersection(start, vrRay))
  {
    
    mouseSelectedEntity = objModel2;
    return;
    
  }*/
  
  mouseSelectedEntity = 0;


  /*glm::vec3 modelPosition = objModel->tranform().position();
	float radius = objModel->boundingVolumenRadius();

	float b = 2 * (vrRay.x * (start.x - modelPosition.x) +
		vrRay.y * (start.y - modelPosition.y) + vrRay.z * (start.z - modelPosition.z));
	float c = start.x * start.x - 2 * start.x * modelPosition.x + modelPosition.x * modelPosition.x
		+ start.y * start.y - 2 * start.y * modelPosition.y + modelPosition.y * modelPosition.y
		+ start.z * start.z - 2 * start.z * modelPosition.z + modelPosition.z * modelPosition.z - radius * radius;

	float discr = (b * b - 4 * c);
	if (discr >= 0)
	{
		if (showRayByRotation)
		{
			rotateObject = true;
		}
		if (showRayByTranslation)
		{
			translateObject = true;
		}
		std::cout << "YES INTERSECTS " << std::endl;
	}
	else
	{
		rotateObject = false;
		translateObject = false;
	}*/
	
	
}

void MyVRApp::renderSphere(const MinVR::VRGraphicsState &renderState)
{
	if (sphereModel)
	{
		simpleColorShader.start();
		simpleColorShader.setUniformMatrix4fv("p", renderState.getProjectionMatrix());
		if (myMode == desktop)
		{
			simpleColorShader.setUniform("v", camera->GetView());
		}
		else
		{
			simpleColorShader.setUniformMatrix4fv("v", renderState.getViewMatrix());
		}
		
		
		//sphereModel->setPosition()
		sphereModel->render(simpleColorShader);
		simpleColorShader.stop();
	}
}

void MyVRApp::renderListOfMeshes(const MinVR::VRGraphicsState & renderState)
{
	if (!loadedModels.empty())
	{

		simpleTextureShader.start();
		simpleTextureShader.setUniformMatrix4fv("p", renderState.getProjectionMatrix());
		
		if ( myMode == desktop)
		{
			simpleTextureShader.setUniform("v", camera->GetView());
		}
		else
		{
      simpleTextureShader.setUniformMatrix4fv("v", renderState.getViewMatrix());
		}
    
    

		//sphereModel->setPosition()
		for (Model* model: loadedModels)
		{
      simpleTextureShader.setUniform("color", model->color());
			//model->render(simpleTextureShader);
      if (model->isSelected())
      {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        simpleTextureShader.setUniformi("intersected", 1);
        model->render(simpleTextureShader);
        glEnable(GL_BLEND);
      }
      else
      {
        simpleTextureShader.setUniformi("intersected", 0);
        model->render(simpleTextureShader);
      }
		}
		//sphereModel->render(simpleColorShader);
		simpleTextureShader.stop();
	}
}

void MyVRApp::menu_callback()
{
  // create a file browser instance
  if (m_is2d) {
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(40, 40), ImGuiCond_Once);
  }

  //...do other stuff like ImGui::NewFrame();

  if (ImGui::Begin("dummy window"))
  {
    // open file dialog when user clicks this button
    if (ImGui::Button("open file dialog")) {
      // (optional) set browser properties
      fileDialog.SetTitle("title");
      fileDialog.SetTypeFilters({ ".obj" });

      fileDialog.Open();
    }
    ImGui::End();


    fileDialog.Display();

    if (fileDialog.HasSelected())
    {
      std::string selectedFile = fileDialog.GetSelected().string();

      std::cout << "Selected filename" << selectedFile << std::endl;
      loadFileModel(selectedFile);

      fileDialog.ClearSelected();
    }

    ////...do other stuff like ImGui::Render();
  }
}

void MyVRApp::menu_callback2()
{
  if (m_is2d) {
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(400, 40), ImGuiCond_Once);
  }
  ImGui::ShowDemoWindow();
}

void MyVRApp::loadFileModel(std::string& fileName)
{
  Model* newObjModel = GLMLoader::loadModel(fileName);
  if (newObjModel)
  {
    newObjModel->tranform().setPosition(glm::vec3(0,0,0));
    newObjModel->tranform().setScale(glm::vec3(0.25, 0.25, 0.25));
    loadedModels.push_back(newObjModel);
  }
  else
  {
    std::cout << "Selected filename" << fileName << "did not load" <<std::endl;
  }
  
  
}

Model* MyVRApp::TestRayEntityIntersection(vec3 ray)
{
 
  /*if (objModel)
  {
    if (objModel->RayInstersection(camera->myPosition, ray))
    {
      return objModel;
    }
  }*/

  for (int i = 0; i < loadedModels.size(); ++i)
  {
    Model* model = loadedModels[i];

    if (model->RayInstersection(camera->myPosition, ray))
    {
      return model;
    }

  }

  return 0;
}

glm::vec3 MyVRApp::RayCast(float mouse_x, float mouse_y)
{
  vec4 rayClip((2.0f * mouse_x) / camera->myWindowWidth - 1.0f,
    1.0f - (2.0f *  mouse_y) / camera->myWindowHeight, -1.0, 1.0);
  mat4 projection = glm::perspective(
    camera->myFoV,(float)camera->myWindowWidth / (float)camera->myWindowHeight, 0.1f, 1000.0f);
  vec4 rayEye = inverse(projection) * rayClip;
  rayEye.z = -1.0f;
  rayEye.w = 0.0f;
  mat4 view = camera->GetView();
  mat4 invView = inverse(view);
  vec4 temp = invView * rayEye;
  vec3 rayWorld(temp.x, temp.y, temp.z);
  rayWorld = normalize(rayWorld);
  return rayWorld;
}

void MyVRApp::onRenderGraphicsContext(const MinVR::VRGraphicsState &renderState)
{
	// Run setup if this is the initial call.
	if (renderState.isInitialRenderCall()) {
    
    //renderState.getValue("");
    MinVR::VRDataIndex index = renderState.index();
    int width = index.getValue("/WindowWidth");
    int height = index.getValue("/WindowHeight");
    

		initGraphicsDrivers();

		initCamera(width, height);

		initModel();

		initShaders();

		

	}
  if (menus)
  {
    menus->renderToTexture();
  }
  
}

void MyVRApp::onRenderGraphicsScene(const MinVR::VRGraphicsState &renderState)
{
	// Only render if running.
	//if (isRunning()) {
		// Clear the screen.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    glDisable(GL_CULL_FACE);


    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(renderState.getProjectionMatrix());

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(renderState.getViewMatrix());
    menus->drawMenu();
    glFlush();
    glEnable(GL_CULL_FACE);


		renderScene(renderState);
	//	camera->Update();
	//}
    //glEnable(GL_NORMALIZE);		glEnable(GL_DEPTH_TEST);		glEnable(GL_COLOR_MATERIAL);
    //glEnable(GL_LIGHTING);		glEnable(GL_LIGHT0);
    //glMatrixMode(GL_PROJECTION);		glLoadMatrixf(renderState.getProjectionMatrix());
    //glMatrixMode(GL_MODELVIEW);		glLoadMatrixf(renderState.getViewMatrix());

    

	/*if (viewer.valid())
	{
		viewer->frame();
	}*/
}

