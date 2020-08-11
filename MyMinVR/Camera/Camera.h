#ifndef CAMERA_H
#define CAMERA_H

#include "../common/common.h"
#define PI 3.14159265f
class Camera
{
	
public:

	enum MovingFlag
	{
		stop = 0,
		forward,
		backward,
		right,
		left,
		Up,
		Down
	};

	Camera(int width, int height,glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 Up = glm::vec3(0.0, 1.0, 0.0));
	~Camera();

	
	void RotateX(float angle);
	void RotateY(float angle);

	// rotate the view according to mouse position. 
	void RotateView(glm::vec2 npos);

	void SetCaRotate(bool canMove);

	void Move(MovingFlag);
	void Stop(MovingFlag);

	void Update();

	glm::vec3 myPosition;
	glm::vec3 myFront;
	glm::vec3 myUp;
	glm::vec3 myRightv;

	glm::mat4 GetView();
	
	glm::mat4 GetView(const float* vm);

	glm::vec3 GetFront();

	float mySpeed;
	float myMouseSpeed;
	float myHorizontalAngle;
	float myVerticalAngle;
	float myFoV;

	int myWindowWidth;
	int myWindowHeight;

private:

	const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	unsigned int  inputFlag;
	void rotate(glm::vec3& vector, float angle, const glm::vec3& axis);
	glm::vec2 lastMousePosition;
	bool myCanRotate;
};

#endif