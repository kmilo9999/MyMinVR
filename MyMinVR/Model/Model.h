#ifndef MODEL_H
#define MODEL_H
#include "../GLMLoader/VertexBuffer.h"
#include "../Texture/Texture.h"
#include "../ShaderProgram/ShaderProgram.h"
#include "Transform.h"

class Model
{
public:
	
	~Model();

	VertexBuffer& objModel() const; 
	void setObjModel(VertexBuffer* val); 


	Texture& texture(int index) const; 
	void addTexture(Texture* val); 
	

	void render(ShaderProgram shader);

	float boundingVolumenRadius() const; 
	void setBoundingVolumenRadius(float val); 

  bool RayInstersection(vec3 start, vec3 rayDirection);

  bool isSelected();
  void seleted(bool );

  Transform& tranform()  { return myTransform; }

  void setTranform(Transform val) { myTransform = val; }

  void addChild(Model* parent);

  void removeChild(int pos);

  void setParentTransform(Transform* parentTransforM);

  void removeParent();

 

private:

	Model();

	VertexBuffer* myObjModel;
	std::vector<Texture*> myTextures;

	friend class GLMLoader;
	friend class AssimpLoader;
	friend class Terrain;

  Transform myTransform;

	float myBoundingVolumenRadius;

  bool mySelected;

  std::vector<Model*> myChildren;

  
};

#endif
