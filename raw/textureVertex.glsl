/*
textureVertex.glsl

Vertex shader for light effects.
Adapted from OpenGL Programming Guide 8th edition sample code 
ch08_lightmodels.cpp function render_vs().

Mike Barnes
11/5/2013
*/

# version 330 core
        
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec3 vNormal;

uniform mat3 NormalMatrix;
uniform mat4 MVP;
  
out vec2 vs_texCoord;      
out vec3 vs_worldpos;
out vec3 vs_normal;
        
void main(void) {
  vec4 position = MVP * vPosition;
  gl_Position = position;
  vs_worldpos = position.xyz;
  vs_normal = NormalMatrix * vNormal; 
  vs_texCoord =vTexCoord;
  }
 