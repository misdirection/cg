#version 330

in vec4 passed_normal;
in vec4 passed_vs_light_pos;
in vec4 passed_vs_vertex_pos;
in vec2 passed_texture_coordinates;

out vec4 out_Color;
uniform vec3 PlanetColor;
uniform int useToonShader;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform mat4 NormalMatrix;
vec3 lightColor = vec3(1.0, 1.0, 1.0);

float ka = 0.4;
float kd = 1.0;
float ks = 0.3;

void main(void)
{
    //light vector: vector from vertex to lightsource

            // compute derivations of the view position
            vec3 p_dx = dFdx(passed_vs_vertex_pos.xyz);
            vec3 p_dy = dFdy(passed_vs_vertex_pos.xyz);

            // compute derivations of the texture coordinate
            vec2 tc_dx = dFdx(passed_texture_coordinates);
            vec2 tc_dy = dFdy(passed_texture_coordinates);
    
            // compute initial tangent and bi-tangent
            vec3 t = normalize( tc_dy.y * p_dx - tc_dx.y * p_dy );
            vec3 b = normalize( tc_dy.x * p_dx - tc_dx.x * p_dy ); // sign inversion

            // get new tangent from a given mesh normal
            vec3 norm = normalize(vec3(passed_normal.xyz));

            vec3 x = cross(norm, t);
            t = cross(x, norm);
            t = normalize(t);
    
            // get updated bi-tangent
            x = cross(b, norm);
            b = cross(norm, x);
            b = normalize(b);
            mat3 tbn = transpose(inverse(mat3(t, b, norm)));
    
            //Extract normal from normal map
            vec2 normalXY = vec2(texture2D(normalTexture,passed_texture_coordinates).rg * 2.0f -1.0f);

            vec3 normalFromMap = normalize(vec3(texture2D(normalTexture, passed_texture_coordinates).rg* 2.0 - 1.0, texture2D(normalTexture, passed_texture_coordinates).b/*sqrt(1 - normalXY.x*normalXY.x + normalXY.y*normalXY.y)*/ ) );
            vec3 correctNormal = normalize((vec4(normalFromMap,1.0)).xyz); 

            vec3 finalNormal = normalize(tbn*correctNormal.xyz);
	
	vec3 N = finalNormal;
    //vec3 N = normalize(passed_normal.xyz);
    vec3 L = vec3(normalize( (passed_vs_light_pos - passed_vs_vertex_pos).xyz ));
    vec3 E = vec3(normalize( (-passed_vs_vertex_pos).xyz ));
    vec3 R = normalize(-reflect(L, N) );
  
    float Iamb = ka; 
    float Idiff = kd * max(dot(N,L), 0.0);
          Idiff = clamp(Idiff, 0.0, 1.0);

    float Ispec = ks * max(0.0, (pow(dot(R,E), 9.0)) );

    out_Color = normalize( (passed_normal + 1.0) / 2.0);
    out_Color = vec4(PlanetColor, 1.0);
    vec3 texture_Color = texture2D(colorTexture, passed_texture_coordinates).xyz;

    if(useToonShader == 0)
    {
        out_Color = vec4(Iamb * texture_Color + 
                         Idiff * texture_Color + 
                         Ispec * texture_Color,
                         1.0);

    }
    else
    {
        float intensity = clamp(Iamb + Idiff + Ispec, 0.0, 1.0);
   
        out_Color = vec4((ceil(intensity * 4)/ 4) * PlanetColor, 1.0);


        if(dot(N,E) < 0.3)
           out_Color = vec4(0.0, 0.0, 0.0, 0.0); 
    }

}
