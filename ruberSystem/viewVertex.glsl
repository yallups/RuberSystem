/*
viewVertex.glsl

Fragment shader for light effects.
Adapted from OpenGL Programming Guide 8th edition sample code 
ch08_lightmodels.cpp function render_vs().

Mike Barnes
9/13/2013
*/

# version 330 core
        
uniform float eyePosition;

uniform mat4 ModelView;
uniform mat4 Projection;
        
in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
  
out vec4 vsColor;      
out vec3 vs_worldpos;
out vec3 vs_normal;
        
void main(void) {
  vec4 position = Projection * ModelView * vPosition;
  gl_Position = position;
  vs_worldpos = position.xyz;
  // mat3(ModelView) is a NormalMatrix
  vs_normal = mat3(ModelView) * vNormal; 
  vsColor = vColor;
  }
 