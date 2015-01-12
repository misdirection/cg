#version 330
uniform sampler2D screenQuadColorTexture;
uniform vec2 screenDimensions;
uniform int useGreyScale;
uniform int useMirror;
layout (location = 0) out vec4 out_Color; //this now writes to our color
                                           //attachment 0
void main (void)
{
//...
float x = 0; //offset of current fragment in horizontal dir
float y = 0; //offset of current fragment in vertical dir
vec2 pos = vec2((gl_FragCoord.xy+vec2(x,y))/screenDimensions).xy;
if(useMirror == 1){pos.x =1-pos.x;}

if(useGreyScale == 1){
vec4 texColor = texture2D(screenQuadColorTexture, pos);
float grey_value = 0.2126 * texColor.x + 0.7152 * texColor.y + 0.0722 *  texColor.z;
out_Color.x = grey_value;
out_Color.y = grey_value;
out_Color.z = grey_value;

}else{
vec4 texColor = texture2D(screenQuadColorTexture, pos);
out_Color = texColor;
}
 

}