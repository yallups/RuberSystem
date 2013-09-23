/*
Shape3D.cpp

Mike Barnes
9/7/2013
*/

# ifndef __INCLUDES465__
# include "../includes465/include465.h"
# define __INCLUDES465__
# endif

class RocketShip {

private :

  int id;
  //glm::mat4 rotationMatrix;
  glm::mat4 scaleMatrix;
  glm::mat4 translationMatrix;
  //glm::vec3 rotationAxis; 
  //float radians; 
  //bool orbital;

public:

  RocketShip() {
    //id = number;  // for debugging
    //int random = rand();   // random num 0..MAXINT

		scaleMatrix = glm::scale(glm::mat4(), glm::vec3(200, 200, 200));  // make Ruber
		translationMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z

		//radians = glm::radians(0.2f);
		
	}
    //rotationMatrix = glm::mat4();  // no initial orientation
    //set cube's  rotation axis and rotation radians
    //rotationAxis = glm::vec3(0,  1, 0);
    // determine rotation type
    //orbital = true;
    }

  glm::mat4 getModelMatrix() {
    if (orbital) // orbital rotation
        return(rotationMatrix * translationMatrix * scaleMatrix);
      else  // center rotation
        return(translationMatrix * rotationMatrix * scaleMatrix);
    }

  void update() {
    //rotationMatrix = glm::rotate(rotationMatrix, radians, rotationAxis);
    translationMatrix = glm::translate(translationMatrix, translation);
    }   
  };  