#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "../common/common.h"

using namespace glm;


class ShaderProgram
{

public:
	ShaderProgram();
	void LoadShaders(const char* vertex_filenname, const char* fragment_filename);
	~ShaderProgram();
	void start();
	void stop();

	void addUniform(const char* name);

	void setUniformi(const char* name, int value);
	void setUniformf(const char* name, float value);
	void setUniform(const char* name, const vec3& vector);
	void setUniform(const char* name, vec4 vector);
  void setUniform(const char* name, const vec4& vector);
	void setUniform(const char* name, const mat4& matrix);
	void setUniformMatrix4fv(const char* name, const GLfloat* matrix);

	GLuint GetProgramId();

private:

	GLuint progarmId;
	void VerifiProgram(GLuint programId);
	bool InUse();
	GLuint LoadShader(const char* vertex_filenname, int type);
	std::unordered_map<std::string, GLint> uniforms;
};

#endif