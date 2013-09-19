/*
Shape3D.cpp

Mike Barnes
9/7/2013
*/

# ifndef __INCLUDES465__
# include "../includes465/include465.h"
# define __INCLUDES465__
# endif

class Shape3D {

private :

  int id;
  glm::mat4 scaleMatrix;
  glm::vec3 rotationAxis; 
  float radians; 
  bool orbital;

public:
	
  glm::mat4 rotationMatrix;
  glm::mat4 translationMatrix;

  Shape3D(int number) {
    id = number;  // for debugging
	switch(id) {
		case 0: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(200, 200, 200));  // make Ruber
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
			//set cube's  rotation axis and rotation radians
			rotationAxis = glm::vec3(0,  1, 0);
			radians = glm::radians(0.2f);
			break;
		case 1: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(20, 20, 20));  // make Unum
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(400, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
			//set cube's  rotation axis and rotation radians
			rotationAxis = glm::vec3(0,  1, 0);
			radians = glm::radians(0.4f); //Rotate around Ruber
			break;
		case 2: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(40, 40, 40));  // make Duo
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(-700, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
			//set cube's  rotation axis and rotation radians
			rotationAxis = glm::vec3(0,  1, 0);
			radians = glm::radians(0.2f); //Rotate around Ruber
			break;
		case 3: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(10, 10, 10));  // make Primus
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(-50, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
			//set cube's  rotation axis and rotation radians
			rotationAxis = glm::vec3(0,  1, 0);
			radians = glm::radians(0.2f); //Rotate around Duo
			break;
		case 4: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(15, 15, 15));  // make Secundus
			translationMatrix = glm::translate(glm::mat4(), glm::vec3(100, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
			//set cube's  rotation axis and rotation radians
			rotationAxis = glm::vec3(0,  1, 0);
			radians = glm::radians(0.4f); //Rotate around Duo
			break;
	}
    rotationMatrix = glm::mat4();  // no initial orientation
    // determine rotation type
    orbital = true;
    }

   glm::mat4 getModelMatrix(glm::mat4 tranMatrix, glm::mat4 rotMatrix) {
    //if (orbital) // orbital rotation
	if(id>2){
		return (rotMatrix * tranMatrix *rotationMatrix * translationMatrix * scaleMatrix);
	}
    return (rotationMatrix * translationMatrix * scaleMatrix);
    //else  // center rotation
	//	return(translationMatrix * rotationMatrix * scaleMatrix);
    }

  void update(int id, glm::mat4 tranMatrix, glm::mat4 rotMatrix) {
	//if( id > 2 ) {
	//	rotationMatrix += rotMatrix;
	//	translationMatrix += tranMatrix;
	//}
	rotationMatrix = glm::rotate(rotationMatrix, radians, rotationAxis);
	
    //translationMatrix = glm::translate(translationMatrix, translation);
    }   
  };  