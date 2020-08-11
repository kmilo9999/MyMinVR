#include "Camera.h"



Camera::Camera(int width, int height, glm::vec3 Postion /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, glm::vec3 front /*= glm::vec3(0.0f, 0.0f, -1.0f)*/, glm::vec3 Up /*= glm::vec3(0.0, 1.0, 0.0)*/)
:myPosition(Postion),
mySpeed(0.1f), myMouseSpeed(0.1f), myFoV(45.0f), inputFlag(MovingFlag::stop), 
lastMousePosition(0.0f, 0.0f),
myCanRotate(false), myFront(normalize(front)), myUp(glm::normalize(Up)),
myWindowWidth(width),
myWindowHeight(height)
{

}

Camera::~Camera()
{
}

void Camera::RotateX(float angle)
{
	glm::vec3 Haxis = glm::cross(yAxis, myFront);
	myRightv = glm::normalize(Haxis);

	rotate(myFront, angle, myRightv);
	myFront = glm::normalize(myFront);
	myUp = glm::normalize(cross(myFront, myRightv));
}

void Camera::RotateY(float angle)
{
	glm::vec3 Haxis = glm::cross(yAxis, myFront);
	myRightv = glm::normalize(Haxis);

	rotate(myFront, angle, yAxis);
	myFront = glm::normalize(myFront);
	myUp = glm::normalize(glm::cross(myFront, myRightv));
}

void Camera::RotateView(glm::vec2 npos)
{
	if (!myCanRotate)
	{
		lastMousePosition.x = npos.x;
		lastMousePosition.y = npos.y;

		return;
	}

	float xAngle = (npos.x - lastMousePosition.x)*myMouseSpeed;
	float yAngle = (npos.y - lastMousePosition.y)*myMouseSpeed;
	RotateX(yAngle); // invert the angle to feel it more natural
	RotateY(-xAngle);
	lastMousePosition.x = npos.x;
	lastMousePosition.y = npos.y;
}

void Camera::SetCaRotate(bool canMove)
{
	myCanRotate = canMove;
}

void Camera::Move(MovingFlag direction)
{
	inputFlag = direction;
}

void Camera::Stop(MovingFlag direction)
{
	inputFlag &= !direction;
}

glm::mat4 Camera::GetView()
{
	// Camera matrix
	return glm::lookAt(myPosition, myPosition + myFront, myUp);
}

glm::mat4 Camera::GetView(const float* vm)
{
	glm::mat4 viewMatrix = glm::mat4(vm[0], vm[1], vm[2], vm[3],
		vm[4], vm[5], vm[6], vm[7],
		vm[8], vm[9], vm[10], vm[11],
		vm[12], vm[13], vm[14], vm[15]);

	//viewMatrix = viewMatrix * glm::rotate(myFront, myUp) * glm::translate(myPosition);
	return viewMatrix;
}

glm::vec3 Camera::GetFront()
{
	return myPosition;
}

void Camera::rotate(glm::vec3& vector, float angle, const glm::vec3& axis)
{
	glm::quat quat_view(0, vector.x, vector.y, vector.z);

	float angleRadians = (angle) * (PI / 180);
	float halfSin = sinf(angleRadians / 2);
	float halfCos = cosf(angleRadians / 2);
	glm::quat rotation(halfCos, axis.x * halfSin, axis.y * halfSin, axis.z * halfSin);
	glm::quat conjugate = inverse(rotation);
	glm::quat q = rotation * quat_view;
	q *= conjugate;

	vector.x = q.x;
	vector.y = q.y;
	vector.z = q.z;

}


void Camera::Update()
{
	if (inputFlag == MovingFlag::forward)
	{
		myPosition += myFront * mySpeed;
	}
	if (inputFlag == MovingFlag::backward)
	{
		myPosition -= myFront * mySpeed;
	}
	if (inputFlag == MovingFlag::Up)
	{
		myPosition += glm::vec3(0,0.25,0) * mySpeed;
	}
	if (inputFlag == MovingFlag::Down)
	{
		myPosition -= glm::vec3(0, 0.25, 0) * mySpeed;
	}
	if (inputFlag == MovingFlag::right)
	{
		myPosition += glm::normalize(glm::cross(myFront, myUp)) * mySpeed;
		std::cout << myPosition.x << " " << myPosition.y << " " << myPosition.z << std::endl;
	}
	if (inputFlag == MovingFlag::left)
	{
		myPosition -= glm::normalize(glm::cross(myFront, myUp)) * mySpeed;
		std::cout << myPosition.x << " "<< myPosition.y << " " << myPosition.z << std::endl;
	}
	
}
