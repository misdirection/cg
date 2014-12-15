#version 330

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Color;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 pass_Color;

void main(void)
{
	vec4 vertexPos = vec4(in_Position, 1.0);
	
	//just set the position
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vertexPos;

	pass_Color = in_Color;
}
