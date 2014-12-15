#version 330

in vec4 passed_normal;
in vec4 passed_vs_light_pos;
in vec4 passed_vs_vertex_pos;
in vec2 passed_texture_coordinates;

out vec4 out_Color;
uniform vec3 PlanetColor;
uniform int useToonShader;

uniform sampler2D colorTexture;
uniform sampler2D glossTexture;

vec3 lightColor = vec3(1.0, 1.0, 1.0);

float ka = 0.4;
float kd = 1.0;
float ks = 0.3;

void main(void)
{

    vec3 N = normalize(passed_normal.xyz);
    vec3 L = normalize( (passed_vs_light_pos - passed_vs_vertex_pos).xyz );
    vec3 E = normalize( (-passed_vs_vertex_pos).xyz );
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
