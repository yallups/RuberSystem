/*
Shape3D.cpp

Mike Barnes
9/7/2013

Ian Graham, Shawn Jones, Ronen Adam
Fall 2013
*/

# ifndef __INCLUDES465__
# include "includes465/include465.h"
# define __INCLUDES465__
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
  glm::vec3 position;
  float boundingRad;

public:

  int missiles;
  int traveled; //Count for distance missile has traveled after being fired
  bool inFlight;// bool for if a missile is in flight we can tell it to keep moving other wise leave it alone.
  bool isDead;

  glm::vec3 missileTarget;

  glm::mat4 getRotationMat(){
	  return rotationMatrix;
  }
  glm::mat4 getTranslationMat(){
	  return translationMatrix;
  }
  float getBoundingRadius() {
	  return boundingRad;
  }
  glm::vec3 getPosition() {
	  return position;
  }
  glm::vec3 getDirection() {
	  return glm::vec3(rotationMatrix[2].x,rotationMatrix[2].y,rotationMatrix[2].z);
  }
  void setRotationMat(glm::mat4 rtMat){
	  rotationMatrix = rtMat;
  }
  void setTranslationMat(glm::mat4 tMat){
	  translationMatrix = tMat;
  }
  void setBoundingRadius(float br){
	  boundingRad = br;
  }
  void setPosition(glm::vec3 pos){
	  position = pos;
  }


  Shape3D(int number) {
		missiles = 0;
		id = number;  // for debugging
		switch(id) {
			case 0: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(200, 200, 200));  // make Ruber
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.11f);
				orbital = false;
				break;
			case 1: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(20, 20, 20));  // make Unum
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(400, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.22f); //Rotate around Ruber
				orbital = true;
				break;
			case 2: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(40, 40, 40));  // make Duo
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(-700, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.11f); //Rotate around Ruber
				orbital = true;
				break;
			case 3: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(10, 10, 10));  // make Primus
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(-50, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.11f); //Rotate around Duo
				orbital = true;
				break;
			case 4: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(15, 15, 15));  // make Segundus
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(100, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.22f); //Rotate around Duo
				orbital = true;
				break;
			case 5: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(5, 5, 5));  // make Warbird
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(500, 0, 500));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.0f); //No Rotation
				orbital = false;
				missiles = 10;
				break;
			case 6:
				scaleMatrix = glm::scale(glm::mat4(), glm::vec3(5, 5, 5));  // make missile site Unum
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(400, 10, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.22f); //Rotate around Ruber
				orbital = true;
				missiles = 5;
				break;
			case 7: scaleMatrix = glm::scale(glm::mat4(), glm::vec3(5, 5, 5));  // make missile site Secundus
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(100, 10, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.22f); //Rotate around Duo
				orbital = true;
				missiles = 5;
				break;
			case 8: case 9: 
				scaleMatrix = glm::scale(glm::mat4(), glm::vec3(10, 10, 10));  // make missile
				translationMatrix = glm::translate(glm::mat4(), glm::vec3(1000000, 0, 0));		// initial placement +/- 500 from origin in X, Y, Z
				//set cube's  rotation axis and rotation radians
				rotationAxis = glm::vec3(0,  1, 0);
				radians = glm::radians(0.0f); //No Rotation
				orbital = false;
				inFlight = false;
				traveled = 0;
				break;
			
		}
		isDead = false;
		rotationMatrix = glm::mat4();  // no initial orientation
	}

   glm::mat4 getModelMatrix() {
	   return getModelMatrix(glm::mat4(),glm::mat4());
   }
   glm::mat4 getModelMatrix(glm::mat4 tranMatrix, glm::mat4 rotMatrix) {
	glm::mat4 pos;
    if (orbital) // orbital rotation
	{
		if((id>2 && id<5) || id == 7){
			pos = (rotMatrix * tranMatrix * rotationMatrix * translationMatrix);
			setPosition(glm::vec3(pos[3].x, pos[3].y, pos[3].z));
			return (rotMatrix * tranMatrix * rotationMatrix * translationMatrix * scaleMatrix);
		} else {
			pos = (rotationMatrix * translationMatrix);
			setPosition(glm::vec3(pos[3].x, pos[3].y, pos[3].z));
			return (rotationMatrix * translationMatrix * scaleMatrix);
		}
	}
    else  // center rotation
		pos = (translationMatrix * rotationMatrix);
		setPosition(glm::vec3(pos[3].x, pos[3].y, pos[3].z));
		return(translationMatrix * rotationMatrix * scaleMatrix);
    }

  void update(int movement, glm::vec3 target) {
	  missileTarget = target;
	  update(movement);
  }
  void update(int movement) {
	rotationMatrix = glm::rotate(rotationMatrix, radians, rotationAxis);

	//Movement will happen in update depending on what key was last pressed before the update
	//1: Move forward, 2: Turn Up, 3: Move backward, 4: Turn Down, 5: Turn left, 6: Roll left, 7: Turn right, 8: Roll right
	if(id == 5) {
		switch(movement) {
		case 1: if(!isDead) moveForward(10); break;
		case 2: turnUp(); break;
		case 3: if(!isDead) moveBackward(); break;
		case 4: turnDown(); break;
		case 5: turnLeft(); break;
		case 6: rollLeft(); break;
		case 7: turnRight(); break;
		case 8: rollRight(); break;
		default: break;
		}
	} else if (id > 7) { // these are missiles
		if (inFlight) { // if the missile is in flight... 
			if(traveled >= 50) {
				detectTarget(); //Find Target and turn to
			}
			moveForward(5); // we will need to do more than move forward. but this is a place to start
			traveled++;
			//printPos(); // if you want a constant update on the position of the missile
			if( traveled > 250) {
				inFlight = !inFlight;
			}
		}
	}
  }  

  
  void printPos() {
	  glm::vec3 pos = getPosition();
	  printf("%d: x=%f, y=%f, z=%f\n", id, pos.x,pos.y,pos.z);
  }

  void moveForward(int step) {
	  glm::vec3 direction = glm::vec3(step*rotationMatrix[2].x,step*rotationMatrix[2].y,step*rotationMatrix[2].z);
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
  void warpToPlanet(glm::mat4 tranMatrix, glm::mat4 rotMatrix) {
	  glm::mat4 translation = glm::translate(glm::mat4(), glm::vec3(0,200.0f, 0));
	  translationMatrix = rotMatrix * tranMatrix * translation;
	  rotationMatrix = glm::rotate(glm::mat4(), PI/2, glm::vec3(1.0f,0.0f,0.0f));
  }
  void gravity() {
	  float distance = glm::distance(glm::vec3(translationMatrix[3].x,translationMatrix[3].y,translationMatrix[3].z), glm::vec3(0,0,0));
	  if(distance > 0) {
		  float gravityVector = 90000 / (distance*distance); 
		  glm::vec3 direction = glm::normalize(glm::vec3(-1*translationMatrix[3].x,-1*translationMatrix[3].y,-1*translationMatrix[3].z));
		  direction = glm::vec3(gravityVector*direction.x, gravityVector*direction.y, gravityVector*direction.z);
		  translationMatrix = glm::translate(translationMatrix, direction);
		  //printf("Direction: %f, %f, %f\n", direction.x, direction.y, direction.z);
	  }
  }

  // hopefully takes in the warbirds position and sets it the this position.
  // however I dont think I am doing the right
  void fire(glm::mat4 rot, glm::mat4 trns) {
	  translationMatrix = trns;
	  rotationMatrix = rot;
	  inFlight = true;
  }

  void detectTarget() { //Find target
	glm::vec3 missileTargetPosition = missileTarget;
	if(glm::distance(getPosition(), missileTargetPosition) < 2000) {
		orientToward();
	}
  }
  void orientToward() { //Turn toward target
	  //printf("T: x=%f, y=%f, z=%f\n", missileTarget.x, missileTarget.y, missileTarget.z);
	  glm::vec3 missileDirection = glm::normalize(glm::vec3(rotationMatrix[2].x, rotationMatrix[2].y, rotationMatrix[2].z));
	  glm::vec3 missileToTarget = glm::normalize(glm::vec3(missileTarget.x-getPosition().x, missileTarget.y-getPosition().y, missileTarget.z-getPosition().z));
	  
	  glm::vec3 normal = glm::cross(missileDirection, missileToTarget);
	  float angleDifference = glm::acos(glm::dot(glm::vec3(0,0,1.0f), missileDirection));
	  if(missileDirection.y + (PI/2) > 0)
		  //angleDifference *= -1.0f;
	  printf("Angle Difference: %f\n", angleDifference);
	  missileToTarget = glm::normalize(glm::vec3((missileToTarget.x * glm::cos(angleDifference)) + (missileToTarget.z * glm::sin(angleDifference)), missileToTarget.y, (missileToTarget.z * glm::cos(angleDifference)) - (missileToTarget.x * glm::sin(angleDifference))));

	  float dotP = glm::dot(missileDirection, missileToTarget);
	  float angle = glm::acos(dotP);
	  float turn = 0.1f;
	  
	  printf("Missile Direction: %f, %f, %f\n",missileDirection.x, missileDirection.y, missileDirection.z);
	  printf("Missile to Target: %f, %f, %f\n",missileToTarget.x, missileToTarget.y, missileToTarget.z);
	  printf("Normal: %f, %f, %f\n", normal.x, normal.y, normal.z);
	  printf("Angle: %f, Turn: %f, Distance: %f\n",angle, turn, glm::distance(glm::vec3(), glm::vec3(missileTarget.x-getPosition().x, missileTarget.y-getPosition().y, missileTarget.z-getPosition().z)));

	  if(angle > PI/18) {
		  if(missileToTarget.x > missileDirection.x) {
			  rotationMatrix = glm::rotate(rotationMatrix, -1*(turn/7), glm::vec3(0,1.0f,0));
		  } else if(missileToTarget.x < missileDirection.x) {
			  rotationMatrix = glm::rotate(rotationMatrix, (turn/7), glm::vec3(0,1.0f,0));
		  }

		  if(missileToTarget.y > missileDirection.y) {
			  rotationMatrix = glm::rotate(rotationMatrix, -1*(turn/7), glm::vec3(1.0f,0,0));
		  } else if(missileToTarget.y < missileDirection.y) {
			  rotationMatrix = glm::rotate(rotationMatrix, (turn/7), glm::vec3(1.0f,0,0));
		  }
	  }
	  if(angle <= PI/18 && angle > 0.0f) {
		  rotationMatrix = glm::rotate(rotationMatrix, glm::radians(angle), normal);
	  }
  }
  };  