#ifndef SHADER_H
#define SHADER_H

#define GLEW_STATIC

class Shader {
public:
	static unsigned loadShader(const char *fileName, unsigned shaderType)/*Takes a filename and a Shadertype and returns an id of the compiled shader; shaderType could be: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRIE_SHADER*/;
private:
	Shader();
};
#endif
