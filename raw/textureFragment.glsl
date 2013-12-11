/*
textureFragment.glsl

Fragment shader for light effects.
Adapted from OpenGL Programming Guide 8th edition sample code 
ch08_lightmodels.cpp function render_fs().

There is a single "headlamp" light source directed into the scene.

Mike Barnes
11/5/2013
*/

# version 330 core
        
in  vec2 vs_texCoord;
out vec4 color;
        
in vec3 vs_worldpos;
in vec3 vs_normal;
        
uniform sampler2D Texture;

// define light propoerties
uniform vec4 color_ambient = vec4(0.2, 0.2, 0.2, 1.0);
uniform vec4 color_diffuse = vec4(0.6, 0.6, 0.6, 1.0);
        
uniform vec3 light_position = vec3(0.0f, 0.0f, 2000.0f);
 

void main(void) {
   float ambient = 1.0f;   // scale the ambient light 
   vec4 texColor = texture(Texture, vs_texCoord);
   vec3 light_direction = normalize(light_position - vs_worldpos);
   vec3 normal = normalize(vs_normal);
   float diffuse = max(0.0, dot(normal, light_direction));
   color = ambient * color_ambient + diffuse * texColor;
   }
