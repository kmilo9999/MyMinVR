#include "../Model/Model.h"
#include "../GLMLoader/GLMLoader.h"
#include <string>
#include <cassert>


Model::Model():myTransform(), myBoundingVolumenRadius(1.0f)
{

}


Model::~Model()
{
	delete myObjModel;
	for (int i = 0 ; i < myTextures.size(); ++i)
	{
		delete myTextures[i];
	}
		
}

VertexBuffer& Model::objModel() const
{
	return *myObjModel;
}

void Model::setObjModel(VertexBuffer* val)
{
	myObjModel = val;
}

Texture& Model::texture(int index) const
{
	return *myTextures[index];
}

void Model::addTexture(Texture* val)
{
	myTextures.push_back(val);
}

void Model::render(ShaderProgram shaderProgram)
{


	shaderProgram.setUniform("m", myTransform.getTransformationMatrix());

	assert(myObjModel && "NO MODEL TO RENDER");
	// bind texture
	for (int i = 0; i < myTextures.size(); ++i)
	{
		myTextures[i]->Bind(i);
	}

	//render geometry
	if (myObjModel)
	{
		myObjModel->render();
	}
}


float Model::boundingVolumenRadius() const
{
	return myBoundingVolumenRadius;
}

void Model::setBoundingVolumenRadius(float val)
{
	myBoundingVolumenRadius = val;
}

bool Model::RayInstersection(vec3 start, vec3 rayDirection)
{
  
  glm::vec3 Position = myTransform.position();

  float b = 2 * (rayDirection.x * (start.x - Position.x) +
    rayDirection.y * (start.y - Position.y) + rayDirection.z * (start.z - Position.z));
  float c = start.x * start.x - 2 * start.x * Position.x + Position.x * Position.x
    + start.y * start.y - 2 * start.y * Position.y + Position.y * Position.y
    + start.z * start.z - 2 * start.z * Position.z + Position.z * Position.z - myBoundingVolumenRadius * myBoundingVolumenRadius;

  float discr = (b * b - 4 * c);
  if (discr < 0)
  {
    mySelected = false;
    
  }
  else
  {
    mySelected = true;
    
  }

  return mySelected;
}


bool Model::isSelected()
{
  return mySelected;
}

void Model::seleted(bool selected)
{
  mySelected = selected;
}

void Model::addChild(Model* child)
{
  child->tranform().setParentTransform(&(this->myTransform));
  myChildren.push_back(child);
  
}

void Model::removeChild(int pos)
{
  if (pos > myChildren.size())
  {
    return;
  }
  
  myChildren[pos]->tranform().setParentTransform(0);
  myChildren.erase(myChildren.begin() + pos);
}

void Model::setParentTransform(Transform* parentTransforM)
{
  this->tranform().setParentTransform(parentTransforM);
}



