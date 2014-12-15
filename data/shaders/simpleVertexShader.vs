#version 330

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TextureCoordinates;

out vec4 passed_normal;
out vec4 passed_vs_light_pos;
out vec4 passed_vs_vertex_pos;
out vec2 passed_texture_coordinates;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

vec4 ws_light_pos = vec4(0.0, 0.0, 0.0, 1.0);

void main(void)
{
	vec4 vertexPos = vec4(in_Position, 1.0);

	vec4 vsVertexPos = ViewMatrix * ModelMatrix * vec4(in_Position, 1.0);

	gl_Position = (ProjectionMatrix * vsVertexPos);

	passed_normal = normalize(NormalMatrix * vec4(in_Normal, 0.0));

	//passed_normal = normalize(NormalMatrix * vec4(vertexPos.xyz, 0.0));

	passed_vs_light_pos = ViewMatrix * ws_light_pos;
	passed_vs_vertex_pos =  vsVertexPos;

	passed_texture_coordinates = in_TextureCoordinates;
}
