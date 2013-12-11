/*

glmUtils465.hpp

GLM utility functions for console applications to test matrix/vector
operations for 465 and for openGL applications that have included
include465.h file.

void show*(....)
glm::vec3 get*(....)
float distance(v1, v2)
bool colinear(v1, v1, e)

Mike Barnes
11/21/2013
*/

// include the glm shader-like math library if needed
# ifndef __INCLUDES465__
# define __GLMUTILS__
# define GLM_FORCE_RADIANS  // use radians not angles
# define GLM_MESSAGES   // compiler messages
# include <glm/glm.hpp>
# include <glm/gtc/constants.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# endif

void showVec3(char * name, glm::vec3 v) {
  printf("%s:  ", name);
  printf("[%8.3f  %8.3f  %8.3f ] \n", v.x, v.y, v.z); 
  }

void showVec4(char * name, glm::vec4 v) {
  printf("%s:  ", name);
  printf("[%8.3f  %8.3f  %8.3f %8.3f ]  \n", v.x, v.y, v.z, v.w); 
  }

void showMat4(char * label, glm::mat4 m) {
  printf("%s:  \n", label);
  printf("    Right      Up       At      Pos \n");
  printf("X %8.3f  %8.3f  %8.3f  %8.3f \n", m[0][0], m[1][0], m[2][0], m[3][0]);
  printf("Y %8.3f  %8.3f  %8.3f  %8.3f \n", m[0][1], m[1][1], m[2][1], m[3][1]);
  printf("Z %8.3f  %8.3f  %8.3f  %8.3f \n", m[0][2], m[1][2], m[2][2], m[3][2]);
  printf("W %8.3f  %8.3f  %8.3f  %8.3f \n", m[0][3], m[1][3], m[2][3], m[3][3]);
  }

// reference vector oriented + horizontal
glm::vec3 getRight(glm::mat4 m) {
  return glm::vec3(m[0][0], m[0][1], m[0][2]); }

// reference vector oriented - horizontal
glm::vec3 getLeft(glm::mat4 m) {
  return glm::vec3(-m[0][0], -m[0][1], -m[0][2]); }

// refernece vector oriented + vertical
glm::vec3 getUp(glm::mat4 m) {
  return glm::vec3(m[1][0], m[1][1], m[1][2]); }

// refernece vector oriented - vertical
glm::vec3 getDown(glm::mat4 m) {
  return glm::vec3(-m[1][0], -m[1][1], -m[1][2]); }

// reference vector oriented + depth
glm::vec3 getOut(glm::mat4 m) {
  return glm::vec3(m[2][0], m[2][1], m[2][2]); }

// reference vector oriented - depth
glm::vec3 getIn(glm::mat4 m) {
  return glm::vec3(-m[2][0],-m[2][1], -m[2][2]); }

glm::vec3 getPosition(glm::mat4 m) {
  return glm::vec3(m[3][0], m[3][1], m[3][2]); }

// distance between two glm::vec3 values
float distance(glm::vec3 p1, glm::vec3 p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)  + pow(p1.z - p2.z, 2));
	}

// v1 and v2 are colinear within an epsilon (rounding) range
// epislon of 0.0 has no range, suggest 0.1 for orient towards
bool colinear(glm::vec3 v1, glm::vec3 v2, double epsilon) {
	glm::vec3 v1t, v2t;
	v1t = glm::abs(glm::normalize(v1));
	v2t = glm::abs(glm::normalize(v2));
	if (glm::distance(v1t, v2t) <= epsilon) return true;
	else return false;
	}


