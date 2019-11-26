#ifndef VRCAMERA_H_
#define VRCAMERA_H_

#include "../common/common.h"

class VRCamera
{
public:
	VRCamera();
	~VRCamera();



	glm::mat4 viewMatrix;
	glm::vec3 carpetPosition; 
	glm::vec3 carpetUp; 
	glm::vec3 carpetScale;
};

#endif // VRCAMERA_H_