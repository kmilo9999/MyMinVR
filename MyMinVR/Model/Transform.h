#ifndef TRANSFORM_H
#define TRANSFORM_H


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{


public:
	
  Transform();
  Transform(glm::vec3, glm::quat, glm::vec3);
  Transform(Transform&);
  ~Transform();
  
  
	

  glm::vec3 position() const { return myPosition; }
  void setPosition(glm::vec3 val) { myPosition = val; }

  glm::quat orientation() const { return myOrientation; }
  void setOrientation(glm::quat val) { myOrientation = val; }

  glm::vec3 scale() const { return myScale; }
  void setScale(glm::vec3 val) { myScale = val; }

  glm::mat4 getTransformationMatrix();

  Transform* parentTransform() const { return myParentTransform; }
  void setParentTransform(Transform* val) { myParentTransform = val; }

  bool hasChanged();

  const glm::mat4& matrixWorld() const { return myMatrixWorld; }
  void setMatrixWorld(const glm::mat4& val) { myMatrixWorld = val; }

  void computerMatrixWorld();
  
  void applyMatrix(glm::mat4 matrix);

private:
  glm::vec3 myPosition;
  glm::quat myOrientation;
  glm::vec3 myScale;

  glm::vec3 oldPosition;
  glm::quat oldOrientation;
  glm::vec3 oldScale;

  Transform* myParentTransform;
  
  glm::mat4 myMatrixWorld;
  
};

#endif

