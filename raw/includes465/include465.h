/*

Include file to set up OS compilation for 465 projects.
comment / uncomment for your operating system

Assumes the "including application) has defined one of the following
"target" operating systems.

Contains several cpp defined constants for project builds.
All *465 constants start and end with 2 underscores.

__Linux__      // Ubuntu or other distro
__MinGW__      // Windows 7, Minimalist Gnu for Windows
__Windows__    // Windows 7, Visual Studio 2010 (or 2012?)
__Xcode__      // Mac OSX 


Includes utility functions to load glsl shaders and 
AC3D *.tri models.


Mike Barnes
9/2/2013
*/

// defines and includes


# define __INCLUDES465__
# define BUFFER_OFFSET(x)  ((const GLvoid *) (x))  // OpenGL PG 8th ed. code, in vgl.h 
# define MAX_INFO_LOG_SIZE 2048  // for error messages in loadShaders(...)

# include <stdio.h>  // My examples use printf, I'm not a fan of cin, cout...
# include <stdlib.h>
# include <sys/stat.h>

# ifdef __Linux__
# include <GL/glew.h>
# include <GL/freeglut.h>
// include the glm shader-like math library
# define GLM_FORCE_RADIANS  // use radians not angles
# define GLM_MESSAGES   // compiler messages
# include <glm/glm.hpp>
# include <glm/gtc/constants.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include "../includes465/shader465.h"    // load vertex and fragment shaders
# include "../includes465/triModel465.h"  // load AC3D *.tri model 
# endif

# ifdef __MinGW__
# endif

# ifdef __Windows__
# include <Windows.h>  
# include <GL/glew.h>
# include <GL/freeglut.h>
// include the glm shader-like math library
# define GLM_FORCE_RADIANS  // use radians not angles
# define GLM_MESSAGES   // compiler messages
# include <glm/glm.hpp>
# include <glm/gtc/constants.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include "../includes465/shader465.h"    // load vertex and fragment shaders
# include "../includes465/triModel465.h"  // load AC3D *.tri model 
# endif

# ifdef __Xcode__      
# include <GLUT/glut.h>
// include the glm shader-like math library
# define GLM_FORCE_RADIANS  // use radians not angles
# define GLM_MESSAGES   // compiler messages
# include <glm/glm.hpp>
# include <glm/gtc/constants.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include "../includes465/shader465.h"    // load vertex and fragment shaders
# include "../includes465/triModel465.h"  // load AC3D *.tri model 
# endif

const float PI = 3.14159f;
  
