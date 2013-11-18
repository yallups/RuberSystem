/*
viewVertex.glsl

Fragment shader for light effects.
Adapted from OpenGL Programming Guide 8th edition sample code 
ch08_lightmodels.cpp function render_vs().

Mike Barnes
8/19/2013
*/

# version 330 core

uniform mat4 Model;
uniform mat4 View;  
uniform mat4 Project;
        
in vec4 vColor;
in vec3 vNormal;
in vec4 vPosition;
  
out vec4 vsColor;  
out vec3 vs_normal;    
out vec3 vs_worldpos;

void main(void) {
  vec4 position = Project * View * Model * vPosition;
  gl_Position = position;
  vs_worldpos = position.xyz;
  vs_normal = mat3(View*Model) * vNormal; 
  vsColor = vColor;
  }
 