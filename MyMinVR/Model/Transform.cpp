#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Transform::Transform():myPosition(), myOrientation(),
myScale(glm::vec3(1, 1, 1)), myParentTransform(0)
{


}

Transform::Transform(glm::vec3 position, glm::quat orientation, glm::vec3 scale)
  :myPosition(position), myOrientation(orientation), myScale(scale), myParentTransform(0)
{

}

Transform::Transform(Transform& t)
  : myPosition(t.myPosition), myOrientation(t.myOrientation),
  myScale(t.myScale)
{
  oldPosition = myPosition;
  oldOrientation = myOrientation;
  myScale = oldScale;
}

Transform::~Transform()
{

}


glm::mat4 Transform::getTransformationMatrix()
{
  /*glm::mat4 m_model = glm::mat4(1.0f);
  glm::mat4 translateM = glm::translate(m_model,myPosition);
  glm::mat4 rotationM = glm::toMat4(myOrientation);
  glm::mat4 scaleM = glm::scale(glm::mat4(1.0f),myScale);*/
   
  if (hasChanged())
  {
    computerMatrixWorld();
  }
  
  glm::mat4 parentMatrix(1.0);

  if (myParentTransform )
  {
    parentMatrix = parentTransform()->getTransformationMatrix();
  }
 

  //return parentMatrix * translateM * rotationM *scaleM;
  return parentMatrix * myMatrixWorld;
}

bool Transform::hasChanged()
{
  if (myPosition != oldPosition)
  {
    return true;
  }
  if (myOrientation != oldOrientation)
  {
    return true;
  }
  if (myScale != oldScale)
  {
    return true;
  }
  

  return false;
}

void Transform::computerMatrixWorld()
{
  glm::mat4 m_model = glm::mat4(1.0f);
  glm::mat4 translateM = glm::translate(m_model, myPosition);
  glm::mat4 rotationM = glm::toMat4(myOrientation);
  glm::mat4 scaleM = glm::scale(glm::mat4(1.0f), myScale);


  myMatrixWorld = translateM * rotationM *scaleM;
  
  oldPosition = myPosition;
  oldOrientation = myOrientation;
  oldScale = myScale;

}

void Transform::applyMatrix(glm::mat4 matrix)
{
  if (hasChanged())
  {
    computerMatrixWorld();
  }
  
  myMatrixWorld = matrix * myMatrixWorld;

  glm::decompose(myMatrixWorld,this->myScale,this->myOrientation,
    this->myPosition,glm::vec3(),glm::vec4());
}