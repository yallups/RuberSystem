/*
Shape3D.cpp

Mike Barnes
9/7/2013
*/

# ifndef __INCLUDES465__
# include "../includes465/include465.h"
# define __INCLUDES465__
# endif
# ifndef __Missile__
# include "Missile.hpp"
# endif
# define __Shape3D__


class Shape3D {

private :

  int id;
  glm::mat4 scaleMatrix;
  glm::mat4 rotationMatrix;
  glm::mat4 translationMatrix;
  glm::vec3 rotationAxis; 
  glm::vec3 translation; 
  float radians; 
  bool orbital;

public:

	int missiles;

  glm::mat4 getRotationMat(){
	  return rotationMatrix;
  }
  glm::mat4 getTranslationMat(){
	  return translationMatrix;
  }
  void setRotationMat(glm::mat4 rtMat){
	  rotationMatrix = rtMat;
  }
  void setTranslationMat(glm::mat4 tMat){
	  translationMatrix = tMat;
  }
  void setPostion(glm::mat4 tm, glm::mat4 rm){
	  translationMatrix = tm;
	  rotationMatrix = rm;
  }

  Shape3D(int number) {
		missiles = 0;
		id = number;  // for debugging
		switch(id) {
			case 0: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(200, 200, 200));  // make Ruber
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.2f);
				orbital = false;
				break;
			case 1: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(20, 20, 20));  // make Unum
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(400, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.4f); //Rotate around Ruber
				orbital = true;
				break;
			case 2: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(40, 40, 40));  // make Duo
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(-700, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.2f); //Rotate around Ruber
				orbital = true;
				break;
			case 3: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(10, 10, 10));  // make Primus
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(-50, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.2f); //Rotate around Duo
				orbital = true;
				break;
			case 4: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(15, 15, 15));  // make Secundus
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(100, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.4f); //Rotate around Duo
				orbital = true;
				break;
			case 5: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(5, 5, 5));  // make Warbird
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(500, 100, 500));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.0f); //No Rotation
				orbital = false;
				missiles = 10;
				break;
			case 6:
				scaleMatrix = glm::scale(glm::mat4(), glm::vec3(20, 20, 20));  // make missle site Unum
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(0, 10, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.0f); //Rotate around Duo
				orbital = true;
				missiles = 5;
				break;
			case 7: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(20, 20, 20));  // make missle site Secundus
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(0, 10, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.0f); //No Rotation
				orbital = true;
				missiles = 5;
				break;
			case 8: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(20, 20, 20));  // make missle1
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.0f); //No Rotation
				orbital = false;
				break;
			case 9: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(20, 20, 20));  // make missle2
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.0f); //No Rotation
				orbital = false;
				break;

		}
		rotationMatrix = glm::mat4();  // no initial orientation
		// determine rotation type
	}

   glm::mat4 getModelMatrix() {
	   return getModelMatrix(glm::mat4(),glm::mat4());
   }
   glm::mat4 getModelMatrix(glm::mat4 tranMatrix, glm::mat4 rotMatrix) {
    if (orbital) // orbital rotation
	{
		if(id>2){
			return (rotMatrix * tranMatrix * rotationMatrix * translationMatrix * scaleMatrix);
		}
		return (rotationMatrix * translationMatrix * scaleMatrix);
	}
    else  // center rotation
		return(translationMatrix * rotationMatrix * scaleMatrix);
    }

  void update() {
	  if (id > 7 ) {
		  // make adjustments to orient in the corrct direction.
		  moveForward();
	  } else {
		rotationMatrix = glm::rotate(rotationMatrix, radians, rotationAxis);
	  }
	//translationMatrix = glm::translate(translationMatrix, translation);
    }  
  void moveForward() {
	  glm::vec3 direction = glm::vec3(10*rotationMatrix[2].x,10*rotationMatrix[2].y,10*rotationMatrix[2].z);
	  translationMatrix = glm::translate(translationMatrix, glm::vec3(direction.x,direction.y,direction.z));
  }
  void turnUp() {
	  rotationAxis = glm::vec3(1.0f,0.0f,0.0f);
	  rotationMatrix = glm::rotate(rotationMatrix, -0.02f, rotationAxis);
  }
  void turnDown() {
	  rotationAxis = glm::vec3(1.0f,0.0f,0.0f);
	  rotationMatrix = glm::rotate(rotationMatrix, 0.02f, rotationAxis);
  }
  void moveBackward() {
	  glm::vec3 direction = glm::vec3(-10*rotationMatrix[2].x,-10*rotationMatrix[2].y,-10*rotationMatrix[2].z);
	  translationMatrix = glm::translate(translationMatrix, glm::vec3(direction.x,direction.y,direction.z));
  }
  void moveLeft() {
	  glm::mat4 rotationMatrix2 = glm::rotate(rotationMatrix, PI/2, glm::vec3(0.0f,1.0f,0.0f));
	  glm::vec3 direction = glm::vec3(10*(rotationMatrix2[2].x), 0, 10*(rotationMatrix2[2].z));
	  translationMatrix = glm::translate(translationMatrix, glm::vec3(direction.x,0,direction.z));
  }
  void turnLeft() {
	  rotationAxis = glm::vec3(0.0f,1.0f,0.0f);
	  rotationMatrix = glm::rotate(rotationMatrix, 0.02f, rotationAxis);
  }
  void rollLeft() {
	  rotationAxis = glm::vec3(0.0f,0.0f,1.0f);
	  rotationMatrix = glm::rotate(rotationMatrix, -0.02f, rotationAxis);
  }
  void moveRight() {
	  glm::mat4 rotationMatrix2 = glm::rotate(rotationMatrix, 3*PI/2, glm::vec3(0.0f,1.0f,0.0f));
	  glm::vec3 direction = glm::vec3(10*(rotationMatrix2[2].x), 0, 10*(rotationMatrix2[2].z));
	  translationMatrix = glm::translate(translationMatrix, glm::vec3(direction.x,0,direction.z));
  }
  void turnRight() {
	  rotationAxis = glm::vec3(0.0f,1.0f,0.0f);
	  rotationMatrix = glm::rotate(rotationMatrix, -0.02f, rotationAxis);
  }
  void rollRight() {
	  rotationAxis = glm::vec3(0.0f,0.0f,1.0f);
	  rotationMatrix = glm::rotate(rotationMatrix, 0.02f, rotationAxis);
  }
  void moveUp() {
	  glm::mat4 rotationMatrix2 = glm::rotate(rotationMatrix, 3*PI/2, glm::vec3(1.0f,0.0f,0.0f));
	  glm::vec3 direction = glm::vec3(0, 10*(rotationMatrix2[2].y), 0);
	  translationMatrix = glm::translate(translationMatrix, glm::vec3(0,direction.y,0));
  }
  void moveDown() {
	  glm::mat4 rotationMatrix2 = glm::rotate(rotationMatrix, PI/2, glm::vec3(1.0f,0.0f,0.0f));
	  glm::vec3 direction = glm::vec3(0, 10*(rotationMatrix2[2].y), 0);
	  translationMatrix = glm::translate(translationMatrix, glm::vec3(0,direction.y,0));
  }
  Shape3D fireMissile(glm::vec3 direction) {
	  if(missiles) {
		   
		  //Make new shape that looks like a missle
		  //Missile missile = new Missile(direction, getModelMatrix());

		  // set it in motion with an constant translation in its positivve direction

		  missiles--;
		  //return the instance
		  //return missile;
	  }
  }
  };  