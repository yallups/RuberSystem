/*
viewFragment.glsl

Fragment shader for light effects.
Adapted from OpenGL Programming Guide 8th edition sample code 
ch08_lightmodels.cpp function render_fs().

There is a single "headlamp" light source directed into the scene.

Mike Barnes
9/13/2013
*/

# version 330 core
        
in  vec4 vsColor;
out vec4 color;
        
in vec3 vs_worldpos;
in vec3 vs_normal;
        
// define light propoerties
uniform vec4 color_ambient = vec4(0.2, 0.2, 0.2, 1.0);
uniform vec4 color_diffuse = vec4(0.4, 0.4, 0.4, 1.0);
uniform vec4 color_specular = vec4(1.0, 1.0, 1.0, 1.0);  
uniform float shininess = 50.0f;
        
uniform vec3 light_position = vec3(0.0f, 0.0f, 0.0f);
 

void main(void) {
   float ambient = 1.0f;   // scale the ambient light 
   vec3 light_direction = normalize(vs_worldpos);
   vec3 normal = normalize(vs_normal);
   vec3 half_vector = normalize(light_direction);
   float diffuse = max(0.0, dot(normal, light_direction));
   float specular = pow(max(0.0, dot(normal, half_vector)), shininess);
   color = ambient * color_ambient + diffuse * vsColor + specular * color_specular;
   }
