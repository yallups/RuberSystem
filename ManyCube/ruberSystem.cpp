/*
ruberSystem.cpp
includes Shape3D.cpp

A modification of manyCubes.cpp written by Mike Barnes on 9/7/2013 that uses C and C++.
This file (ruberSystem.cpp) is a C file -- it defines no classes
but has the Ruber Solar System, a Warbird, and Missile Sites with missiles.

File Shape3D defines a 3D shape that can translate and rotate.

465 utility include files:  shader465.h, triModel465.h  
Shaders:  simpleVertex.glsl  amd simpleFragment.glsl

User commands:
'v' changes the camera to view from either the top, front or warbird.
'p' changes the camera to view either planet Unum or planet Duo
'w' warps the warbird to either planet Unum or planet Duo
't' changes the Time Quantum to either ACE, PILOT, TRAINEE, or DEBUG
'f' fires a missile from the warbird

Current state is displayed in the window title.

*/


# define __Windows__ // define your target operating system
# include "../includes465/include465.h"
# define __INCLUDES465__   

# ifndef __Shape3D__
# include "Shape3D.hpp"
# endif

# ifndef __Missile__
# include "Missile.hpp"
# endif

// Constants
const int ACE = 40, PILOT = 100, TRAINEE = 250, DEBUG = 500;
const int RUBER = 0, UNUM = 1, DUO = 2, PRIMUS = 3, SECUNDUS = 4,
	WARBIRD = 5, UNUM_SITE = 6, SECUNDUS_SITE = 7, PLAYER_MISSILE = 8, ENEMY_MISSILE = 9;

// Shapes and models.
const int nShapes = 10;
Shape3D * shape[nShapes];

char * modelFile[] = {
	"sphere_mr.tri", 
	"sphere_mr.tri", 
	"sphere_mr.tri", 
	"sphere_mr.tri", 
	"sphere_mr.tri", 
	"ship03_color.tri", 
	"missile_site.tri", 
	"missile_site.tri",
	"missile.tri", 
	"missile.tri"
};

// Vertices must be multiplied by 3 as there are 3 vertices per line (surface) of model file.
const GLuint nVerticesSphere = 4900 * 3;  // Planets
const GLuint nVerticesWarbird = 980 * 3;  // Warbird (Player's ship)
const GLuint nVerticesMissileSite = 600 * 3; // Missile launch sites (Enemies)
// temporarily I am using the ugly-ass rocket .tri as a missile until we get a real one
const GLuint nVerticesMissile = 144 * 3; // Missile (Same model used for player and enemies)

float boundingRadius[nShapes];  // modelFile's bounding radius
int Index =  0;  // Global variable indexing into VBO arrays.

// display state and "state strings" for title display
// window title strings
char baseStr[60] = "465 Ruber Solar System {v, p, w, f, t} :";
char fpsStr[15], viewStr[15] = " Front View";
char titleStr [100]; 

char viewCase = 'f';
char warpCase = 'u';

GLuint vao[nShapes];  // VertexArrayObject
GLuint buffer[nShapes]; // Create and initialize a buffer object
GLuint shaderProgram; 
char * vertexShaderFile = "simpleVertex.glsl";
char * fragmentShaderFile = "simpleFragment.glsl";
GLuint Model, ViewProj ;  // Mode, View*Projection handles

glm::mat4 projectionMatrix;     // set in reshape()
glm::mat4 modelMatrix;          // set in shape[i]-->updateDraw()
glm::mat4 viewMatrix;           // set in keyboard()
glm::mat4 viewProjectionMatrix; // set in display();

GLuint vPosition[nShapes];
GLuint vColor[nShapes];
GLuint vNormal[nShapes];

// vectors and values for lookAt
glm::vec3 eye, at, up;

// vectors for "modelPlanets"
glm::vec4 vertexSphere[nVerticesSphere];
glm::vec3 normalSphere[nVerticesSphere];
glm::vec4 diffuseColorMaterialSphere[nVerticesSphere];

// vectors for "modelWarbird"
glm::vec4 vertexWarbird[nVerticesWarbird];
glm::vec3 normalWarbird[nVerticesWarbird];
glm::vec4 diffuseColorMaterialWarbird[nVerticesWarbird];

// vectors for "modelMissileSites"
glm::vec4 vertexMissileSite[nVerticesMissileSite];
glm::vec3 normalMissileSite[nVerticesMissileSite];
glm::vec4 diffuseColorMaterialMissileSite[nVerticesMissileSite];

// vectors for "Missiles"
glm::vec4 vertexMissile[nVerticesMissile];
glm::vec3 normalMissile[nVerticesMissile];
glm::vec4 diffuseColorMaterialMissile[nVerticesMissile];

// rotation variables
glm::mat4 identity(1.0f); 
glm::mat4 rotation;

// Game environment settings.
int timerDelay = ACE, frameCount = 0;  // Set initial game speed.

int movementDirection = 0;
boolean gravity = false;  // Set initial gravity state.

// Initialize the game environment.
void init (void) {
	for(int i = 0; i < nShapes; i++) {
		if (i < WARBIRD) {  // Initialize planetary bodies.
			boundingRadius[i] = loadTriModel(modelFile[i], nVerticesSphere, vertexSphere, diffuseColorMaterialSphere, normalSphere);
			if (boundingRadius[i] == -1.0f) {
				printf("loadTriModel error:  returned -1.0f \n");
				exit(1); }
			else
				printf("loaded %s model with %7.2f bounding radius \n", modelFile[i], boundingRadius[i]);

			shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
			glUseProgram(shaderProgram);

			glGenVertexArrays( 1, &(vao[i]) );
			glBindVertexArray( vao[i] );

			glGenBuffers( 1, &(buffer[i]) );
			glBindBuffer( GL_ARRAY_BUFFER, buffer[i] );
			glBufferData( GL_ARRAY_BUFFER, sizeof(vertexSphere) + sizeof(diffuseColorMaterialSphere) + sizeof(normalSphere), NULL, GL_STATIC_DRAW );
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertexSphere), vertexSphere );
			glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertexSphere), sizeof(diffuseColorMaterialSphere), diffuseColorMaterialSphere );
			glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertexSphere) + sizeof(diffuseColorMaterialSphere), sizeof(normalSphere), normalSphere );

			// set up vertex arrays (after shaders are loaded)
			vPosition[i] = glGetAttribLocation( shaderProgram, "vPosition" );
			glEnableVertexAttribArray( vPosition[i] );
			glVertexAttribPointer( vPosition[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

			vColor[i] = glGetAttribLocation( shaderProgram, "vColor" );
			glEnableVertexAttribArray( vColor[i] );
			glVertexAttribPointer( vColor[i], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexSphere)) );

			vNormal[i] = glGetAttribLocation( shaderProgram, "vNormal" );
			glEnableVertexAttribArray( vNormal[i]);
			glVertexAttribPointer( vNormal[i], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexSphere) + sizeof(diffuseColorMaterialSphere)) );

			Model = glGetUniformLocation(shaderProgram, "ModelView");
			ViewProj = glGetUniformLocation(shaderProgram, "Projection");
		} else if(i == WARBIRD) {  // Initialize warbird.
			boundingRadius[i] = loadTriModel(modelFile[i], nVerticesWarbird, vertexWarbird, diffuseColorMaterialWarbird, normalWarbird);

			if (boundingRadius[i] == -1.0f) {
				printf("loadTriModel error:  returned -1.0f \n");
				exit(1); }
			else
				printf("loaded %s model with %7.2f bounding radius \n", modelFile[i], boundingRadius[i]);
			
			boundingRadius[i] = boundingRadius[i] + 5.0f; //Warbird has larger radius

			shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
			glUseProgram(shaderProgram);

			glGenVertexArrays( 1, &(vao[i]) );
			glBindVertexArray( vao[i] );

			glGenBuffers( 1, &(buffer[i]) );
			glBindBuffer( GL_ARRAY_BUFFER, buffer[i] );
			glBufferData( GL_ARRAY_BUFFER, sizeof(vertexWarbird) + sizeof(diffuseColorMaterialWarbird) + sizeof(normalWarbird), NULL, GL_STATIC_DRAW );
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertexWarbird), vertexWarbird );
			glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertexWarbird), sizeof(diffuseColorMaterialWarbird), diffuseColorMaterialWarbird );
			glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertexWarbird) + sizeof(diffuseColorMaterialWarbird), sizeof(normalWarbird), normalWarbird );

			// set up vertex arrays (after shaders are loaded)
			vPosition[i] = glGetAttribLocation( shaderProgram, "vPosition" );
			glEnableVertexAttribArray( vPosition[i] );
			glVertexAttribPointer( vPosition[i], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

			vColor[i] = glGetAttribLocation( shaderProgram, "vColor" );
			glEnableVertexAttribArray( vColor[i] );
			glVertexAttribPointer( vColor[i], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexWarbird)) );

			vNormal[i] = glGetAttribLocation( shaderProgram, "vNormal" );
			glEnableVertexAttribArray( vNormal[i]);
			glVertexAttribPointer( vNormal[i], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexWarbird) + sizeof(diffuseColorMaterialWarbird)) );

			Model = glGetUniformLocation(shaderProgram, "ModelView");
			ViewProj = glGetUniformLocation(shaderProgram, "Projection");
		} else if (i > WARBIRD && i < PLAYER_MISSILE) {  // Initialize missile sites.
			boundingRadius[i] = loadTriModel(modelFile[i], nVerticesMissileSite, vertexMissileSite, diffuseColorMaterialMissileSite, normalMissileSite);
			if (boundingRadius[i] == -1.0f) {
				printf("loadTriModel error:  returned -1.0f \n");
				exit(1); }
			else
				printf("loaded %s model with %7.2f bounding radius \n", modelFile[i], boundingRadius[i]);
		
			boundingRadius[i] = boundingRadius[i] + 3.0f;

			shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
			glUseProgram(shaderProgram);

			glGenVertexArrays( 1, &(vao[i]) );
			glBindVertexArray( vao[i] );

			glGenBuffers( 1, &(buffer[i]) );
			glBindBuffer( GL_ARRAY_BUFFER, buffer[i] );
			glBufferData( GL_ARRAY_BUFFER, sizeof(vertexMissileSite) + sizeof(diffuseColorMaterialMissileSite) + sizeof(normalMissileSite), NULL, GL_STATIC_DRAW );
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertexMissileSite), vertexMissileSite );
			glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertexMissileSite), sizeof(diffuseColorMaterialMissileSite), diffuseColorMaterialMissileSite );
			glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertexMissileSite) + sizeof(diffuseColorMaterialMissileSite), sizeof(normalMissileSite), normalMissileSite );

			// set up vertex arrays (after shaders are loaded)
			vPosition[i] = glGetAttribLocation( shaderProgram, "vPosition" );
			glEnableVertexAttribArray( vPosition[i] );
			glVertexAttribPointer( vPosition[i], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

			vColor[i] = glGetAttribLocation( shaderProgram, "vColor" );
			glEnableVertexAttribArray( vColor[i] );
			glVertexAttribPointer( vColor[i], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexMissileSite)) );

			vNormal[i] = glGetAttribLocation( shaderProgram, "vNormal" );
			glEnableVertexAttribArray( vNormal[i]);
			glVertexAttribPointer( vNormal[i], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexMissileSite) + sizeof(diffuseColorMaterialMissileSite)) );

			Model = glGetUniformLocation(shaderProgram, "ModelView");
			ViewProj = glGetUniformLocation(shaderProgram, "Projection");
		} else {  // Initialize missiles.
			boundingRadius[i] = loadTriModel(modelFile[i], nVerticesMissile, vertexMissile, diffuseColorMaterialMissile, normalMissile);
			if (boundingRadius[i] == -1.0f) {
				printf("loadTriModel error:  returned -1.0f \n");
				exit(1); }
			else
				printf("loaded %s model with %7.2f bounding radius \n", modelFile[i], boundingRadius[i]);

			shaderProgram = loadShaders(vertexShaderFile, fragmentShaderFile);
			glUseProgram(shaderProgram);

			glGenVertexArrays( 1, &(vao[i]) );
			glBindVertexArray( vao[i] );

			glGenBuffers( 1, &(buffer[i]) );
			glBindBuffer( GL_ARRAY_BUFFER, buffer[i] );
			glBufferData( GL_ARRAY_BUFFER, sizeof(vertexMissile) + sizeof(diffuseColorMaterialMissile) + sizeof(normalMissile), NULL, GL_STATIC_DRAW );
			glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertexMissile), vertexMissile );
			glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertexMissile), sizeof(diffuseColorMaterialMissile), diffuseColorMaterialMissile );
			glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertexMissile) + sizeof(diffuseColorMaterialMissile), sizeof(normalMissile), normalMissile );

			// set up vertex arrays (after shaders are loaded)
			vPosition[i] = glGetAttribLocation( shaderProgram, "vPosition" );
			glEnableVertexAttribArray( vPosition[i] );
			glVertexAttribPointer( vPosition[i], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

			vColor[i] = glGetAttribLocation( shaderProgram, "vColor" );
			glEnableVertexAttribArray( vColor[i] );
			glVertexAttribPointer( vColor[i], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexMissile)) );

			vNormal[i] = glGetAttribLocation( shaderProgram, "vNormal" );
			glEnableVertexAttribArray( vNormal[i]);
			glVertexAttribPointer( vNormal[i], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertexMissile) + sizeof(diffuseColorMaterialMissile)) );

			Model = glGetUniformLocation(shaderProgram, "ModelView");
			ViewProj = glGetUniformLocation(shaderProgram, "Projection");
		}
	}

	// Initially use a front view.
	eye = glm::vec3(0.0f, 0.0f, 4000.0f);   // 'eye' is 4000 "out of screen" from the origin.
	at  = glm::vec3(0.0f, 0.0f,    0.0f);   // Looking at the origin.
	up  = glm::vec3(0.0f, 1.0f,    0.0f);   // Camera's 'up' vector.
	viewMatrix = glm::lookAt(eye, at, up);

	// set render state values
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// create shape
	for(int i = 0; i < nShapes; i++) {
		shape[i] = new Shape3D(i);
		shape[i]->setBoundingRadius(boundingRadius[i]);
	}

	printf("%d Shapes created. \n", nShapes);
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	projectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat) width /  (GLfloat) height, 1.0f, 10000.0f); 
}

// update and display animation state in window title
void updateTitle() {
	char str[50];
	//sprintf(str, "eye: X:%s Y:%s Z:", eye.x, eye.y, eye.z);
	strcpy(titleStr, baseStr);
	strcat(titleStr, viewStr);
	strcat(titleStr, fpsStr);
	//strcat(titleStr, str);
	// printf("title string = %s \n", titleStr);
	glutSetWindowTitle( titleStr);
}

void updateView() {
	glm::mat4 unum = shape[1]->getTranslationMat();
	glm::mat4 unumrot = shape[1]->getRotationMat();
	unum = unumrot*unum;

	glm::mat4 duo = shape[2]->getTranslationMat();
	glm::mat4 duorot = shape[2]->getRotationMat();
	duo = duorot*duo;

	glm::mat4 warbird = shape[5]->getTranslationMat();
	glm::mat4 warbirdrot = shape[5]->getRotationMat();

	glm::mat4 eyePosition = glm::translate(glm::mat4(), glm::vec3(0.0f, 50.0f,-120.0f)); //translate backward from warbird
	glm::mat4 atPosition = glm::translate(glm::mat4(), glm::vec3(0.0f, 50.0f, 0.0f));

	warbird = warbird * warbirdrot;
	eyePosition = warbird * eyePosition;
	atPosition = warbird * atPosition;

	switch(viewCase) {
	case 'f' :
		eye = glm::vec3(0.0f, 0.0f, 4000.0f);   // eye is 4000 "out of screen" from origin
		at  = glm::vec3(0.0f, 0.0f,    0.0f);   // looking at origin
		up  = glm::vec3(0.0f, 1.0f,    0.0f);   // camera'a up vector
		viewMatrix = glm::lookAt(eye, at, up);
		strcpy(viewStr, " Front View"); 
		break;
	case 't' :  // top view
		eye = glm::vec3(0.0f, 2000.0f,  0.0f);   // eye is 4000 up from origin
		at  = glm::vec3(0.0f,    0.0f,  0.0f);   // looking at origin  
		up  = glm::vec3(0.0f,    0.0f, -1.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up);
		strcpy(viewStr, " Top View"); 
		break;
	case 'b' :  // warbird view
		eye = glm::vec3(eyePosition[3].x, eyePosition[3].y, eyePosition[3].z);   // eye is 3000 up from origin
		at  = glm::vec3(atPosition[3].x, atPosition[3].y, atPosition[3].z);   // looking at origin  
		up  = glm::vec3(warbird[1].x, warbird[1].y, warbird[1].z);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up); 
		strcpy(viewStr, " Warbird View"); 
		break;
	case 'u' :// unum view
		eye = glm::vec3(unum[3].x,  300.0f, unum[3].z);   // eye is 3000 up from origin
		at  = glm::vec3(unum[3].x,   0.0f,  unum[3].z);   // looking at origin  
		up  = glm::vec3(0.0f,        0.0f,      -1.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up); 
		strcpy(viewStr, " Unum View"); 
		break;
	case 'd' : // duo view
		eye = glm::vec3(duo[3].x,  300.0f, duo[3].z);   // eye is 3000 up from origin
		at  = glm::vec3(duo[3].x,    0.0f, duo[3].z);   // looking at origin  
		up  = glm::vec3(0.0f,        0.0f,    -1.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up);
		strcpy(viewStr, " Duo View"); 
		break;
	}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// update model matrix, set MVP, draw
	updateView();

	for (int i = 0; i < nShapes; i++) { 
		modelMatrix = shape[i]->getModelMatrix(shape[2]->getTranslationMat(), shape[2]->getRotationMat()); 
		if (i > SECUNDUS_SITE) {
			modelMatrix = shape[i]->getModelMatrix(); 
		}

		glBindVertexArray( vao[i] );
		viewProjectionMatrix = projectionMatrix * viewMatrix; 
		glEnableVertexAttribArray( vPosition[i]);
		glEnableVertexAttribArray( vColor[i]);
		glEnableVertexAttribArray( vNormal[i]);
		glUniformMatrix4fv(Model, 1, GL_FALSE, glm::value_ptr(modelMatrix)); 
		glUniformMatrix4fv(ViewProj, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));

		if (i < WARBIRD)  // Draw planetary bodies.
			glDrawArrays(GL_TRIANGLES, 0, nVerticesSphere);
		else if (i == WARBIRD)  // Draw warbird.
			glDrawArrays(GL_TRIANGLES, 0, nVerticesWarbird);
		else if (i > WARBIRD && i < PLAYER_MISSILE)  // Draw missile sites.
			glDrawArrays(GL_TRIANGLES, 0, nVerticesMissileSite);
		else
			glDrawArrays(GL_TRIANGLES, 0, nVerticesMissile);  // Draw missiles.
	}
	glutSwapBuffers();
}

// Determine if an object has collided with another object.
boolean detectCollision(float br1, glm::vec3 pos1, float br2, glm::vec3 pos2) {
	float d = glm::distance(pos1, pos2);  //sqrtf(pow((pos1.x - pos2.x),2) + pow((pos1.y - pos2.y),2) + pow((pos1.z - pos2.z),2));
	return (d - (br1 + br2) <= 0);
}

// Check for collisions between objects.
void checkCollides(){
	// Specify the objects with which some objects may collide.
	int warbirdObjects[8] = {RUBER, UNUM, DUO, PRIMUS, SECUNDUS, UNUM_SITE, SECUNDUS_SITE, ENEMY_MISSILE};
	int missileSiteObjects[2] = {WARBIRD, PLAYER_MISSILE};
	int playerMissileObjects[7] = {RUBER, DUO, PRIMUS, SECUNDUS, UNUM_SITE, SECUNDUS_SITE, ENEMY_MISSILE};
	int enemyMissileObjects[6] = {RUBER, UNUM, DUO, PRIMUS, WARBIRD, PLAYER_MISSILE};

	for(int i = RUBER; i < PLAYER_MISSILE; i++) {  // Check if the warbird has collided with an object.
		if(detectCollision(shape[5]->getBoundingRadius(), shape[5]->getPosition(),shape[warbirdObjects[i]]->getBoundingRadius(),shape[warbirdObjects[i]]->getPosition()))
		{
			printf("You have died!\n");
			shape[5]->isDead = true;
		}
	}

	for(int i = 0; i < SECUNDUS_SITE; i++) {  // Check if the player's missile has collided with an object.
		if(detectCollision(shape[8]->getBoundingRadius(), shape[8]->getPosition(), shape[playerMissileObjects[i]]->getBoundingRadius(), shape[playerMissileObjects[i]]->getPosition()) && shape[8]->traveled > 10)
		{
			shape[8]->inFlight = false;
		}
	}

	for(int i = 0; i < UNUM_SITE; i++) {  // Check if the enemy missile has collided with an object.
		if(detectCollision(shape[9]->getBoundingRadius(), shape[9]->getPosition(), shape[enemyMissileObjects[i]]->getBoundingRadius(), shape[enemyMissileObjects[i]]->getPosition()) && shape[9]->traveled > 10)
		{
			shape[9]->inFlight = false;
		}
	}

	for(int i = 0; i < DUO; i++) {
		// Check if the Unum missile site has collided with an object.
		if(detectCollision(shape[6]->getBoundingRadius(), shape[6]->getPosition(), shape[missileSiteObjects[i]]->getBoundingRadius(), shape[missileSiteObjects[i]]->getPosition()))
		{
			if(!(shape[8]->inFlight && i == PLAYER_MISSILE)) {	//If isn't missile nor missile is in flight
			} else if(shape[5]->isDead) {			//If warbird is dead
			} else {
				printf("Unum Missile Site Hit!\n");
				shape[6]->isDead = true;
			}
		}
		// Check if the Secundus missile site has collided with an object.
		if(detectCollision(shape[7]->getBoundingRadius(), shape[7]->getPosition(), shape[missileSiteObjects[i]]->getBoundingRadius(), shape[missileSiteObjects[i]]->getPosition()) && shape[8]->inFlight)
		{
			if(!(shape[8]->inFlight && i == 8)) {	//If isn't missile nor missile is in flight
			} else if (shape[5]->isDead) {			//If warbird is dead
			} else {
				printf("Secundus Missile Site Hit!\n");
				shape[7]->isDead = true;
			}
		}
	}
}

// If the warbird is in range, fire an enemy missile.
void missileSiteDetection() {
	
	if(glm::distance(shape[6]->getPosition(), shape[5]->getPosition()) < 500 && shape[6]->missiles > 0 && !shape[9]->inFlight && !shape[6]->isDead && (shape[9]->traveled > 250 || shape[9]->traveled == 0)) {
		shape[9]->traveled = 0;
		glm::mat4 direction = glm::mat4();
		direction[2] = glm::normalize(glm::vec4(shape[5]->getPosition() - shape[6]->getPosition(), 0.0f));
		glm::mat4 theposition = glm::translate(glm::mat4(), shape[6]->getPosition());
		shape[9]->fire(direction , theposition);
		shape[6]->missiles--;
	} else if(glm::distance(shape[7]->getPosition(), shape[5]->getPosition()) < 500 && shape[7]->missiles > 0 && !shape[9]->inFlight && !shape[7]->isDead && (shape[9]->traveled > 250 || shape[9]->traveled == 0)) {
		shape[9]->traveled = 0;
		glm::mat4 direction = glm::mat4();
		direction[2] = glm::normalize(glm::vec4(shape[5]->getPosition() - shape[7]->getPosition(), 0.0f));
		glm::mat4 theposition = glm::translate(glm::mat4(), shape[7]->getPosition());
		shape[9]->fire(direction , theposition);
		shape[7]->missiles--;
	}
}


// for use with Idle and intervalTimer functions 
// to set rotation
void animate(void){}

//void checkCollisons() {
//
//	glm::vec3 pos1;
//	//glm::vec4 shipPos = shape[5]->getModelMatrix(shape[2]->getTranslationMat(), shape[2]->getRotationMat())[3]; 
//	
//	glm::vec3 shipPos = shape[5]->getposition();
//	
//	glm::vec3 goodMissilePos = shape[8]->getposition();
//	glm::vec3 badMissilePos = shape[9]->getposition();
//	glm::vec3 missileSite1 = shape[6]->getposition();
//	glm::vec3 missilwSite2 = shape[7]->getposition();
//
//	float d;
//
//	for(int i = 0; i < 5; i++) {	
//		pos1 = shape[i]->getposition(); 
//		
//		d = sqrtf(pow((pos1.x - shipPos.x),2) + pow((pos1.y - shipPos.y),2) + pow((pos1.z - shipPos.z),2));
//
//		if (d - (boundingRadius[i] + boundingRadius[5]) <= 0) {
//			printf("YOU LOSE!!! BOOOOM! %d <--> %d\n", i, 5);
//		}
//	}
//
//	d = sqrtf(pow((badMissilePos.x - shipPos.x),2) + pow((badMissilePos.y - shipPos.y),2) + pow((badMissilePos.z - shipPos.z),2));
//	if (d - (boundingRadius[9] + boundingRadius[5]) <= 0) {
//		printf("YOU LOSE!!!! BOOOOM! %d <--> %d\n", 9, 5);
//	}
//
//	d = sqrtf(pow((goodMissilePos.x - missileSite1.x),2) + pow((goodMissilePos.y - missileSite1.y),2) + pow((goodMissilePos.z - missileSite1.z),2));
//	if (d - (boundingRadius[8] + boundingRadius[6]) <= 0) {
//		printf("got one: BOOOOM! %d <--> %d\n", 8, 6);
//	}
//
//	d = sqrtf(pow((goodMissilePos.x - missilwSite2.x),2) + pow((goodMissilePos.y - missilwSite2.y),2) + pow((goodMissilePos.z - missilwSite2.z),2));
//	if (d - (boundingRadius[8] + boundingRadius[7]) <= 0) {
//		printf("got one: BOOOOM! %d <--> %d\n", 8, 7);
//	}
//	
//
//}


// Estimate FPS, use fixed interval timer to measure every second
// timerDelay = 1000
void intervalTimer (int i) { 
	glutTimerFunc(timerDelay, intervalTimer, 1);
	// display frames / second
	sprintf(fpsStr, " FPS %4d", frameCount );
	frameCount = 1000/timerDelay;
	updateTitle();
	glutPostRedisplay();
	for(int i = 0; i < nShapes; i++) {
		if(i < 8) {
			shape[i] -> update(movementDirection);
		} else if(i == 8 && shape[i]->inFlight) {
			float site1 = glm::distance(shape[i]->getPosition(),shape[6]->getPosition());
			
			float site2 = glm::distance(shape[i]->getPosition(),shape[7]->getPosition());
			
			if(shape[6]->isDead) site1 = 1000000.0f;
			else if(shape[7]->isDead) site2 = 1000000.0f;

			if(site1 <= site2) {
				printf("Target = Unum\n");
				shape[i] -> update(movementDirection,shape[6]->getPosition());
			} else {
				printf("Target = Segundus\n");
				shape[i] -> update(movementDirection,shape[7]->getPosition());
			}
		} else if(i == 9 && shape[i]->inFlight) {
			shape[i] -> update(movementDirection,shape[5]->getPosition());
		}
		if(gravity && (i == 5 || i > 7)) {
			shape[i]->gravity(); 
		}
	}
	missileSiteDetection();
	checkCollides();
	movementDirection = 0;
}

void process_SHIFT_ALT_CTRL(int key, int x, int y) 
{
	int mod = glutGetModifiers();

	glm::mat4 unum = shape[1]->getTranslationMat();
	glm::mat4 unumrot = shape[1]->getRotationMat();
	unum = unumrot*unum;

	glm::mat4 duo = shape[2]->getTranslationMat();
	glm::mat4 duorot = shape[2]->getRotationMat();
	duo = duorot*duo;

	glm::mat4 warbird = shape[5]->getTranslationMat();
	glm::mat4 warbirdrot = shape[5]->getRotationMat();
	warbird = warbirdrot*warbird;

	//printf("%d, %d, process_SHIFT_ALT_CTRL\n", mod, key);

	switch(key) {
	case GLUT_KEY_UP :
		if(mod != 0)
			movementDirection = 2;//shape[5]->turnUp();
		else
			movementDirection = 1;//shape[5]->moveForward();
		break;
	case GLUT_KEY_LEFT :
		if(mod == 0)
			movementDirection = 5;//shape[5]->turnLeft();
		else
			movementDirection = 6;//shape[5]->rollLeft();
		break;
	case GLUT_KEY_RIGHT :
		if(mod == 0)
			movementDirection = 7;//shape[5]->turnRight();
		else
			movementDirection = 8;//shape[5]->rollRight();
		break;
	case GLUT_KEY_DOWN:
		if(mod != 0)
			movementDirection = 4;//shape[5]->turnDown();
		else
			movementDirection = 3;//shape[5]->moveBackward();
		break;
	}
}


void keyboard (unsigned char key, int x, int y) {
	int mod = glutGetModifiers();

	glm::mat4 unum = shape[1]->getTranslationMat();
	glm::mat4 unumrot = shape[1]->getRotationMat();
	unum = unumrot*unum;

	glm::mat4 duo = shape[2]->getTranslationMat();
	glm::mat4 duorot = shape[2]->getRotationMat();
	duo = duorot*duo;

	glm::mat4 warbird = shape[5]->getTranslationMat();
	glm::mat4 warbirdrot = shape[5]->getRotationMat();
	warbird = warbirdrot*warbird;

	//printf("%d, %d, keyboard\n", mod, key);

	switch(key) {
	/*case '8' :
		if(mod != 0)
			shape[5]->turnUp();
		else
			shape[5]->moveForward();
		break;
	case '7' :
		shape[5]->moveLeft();
		break;
	case '4' :
		shape[5]->turnLeft();
		break;
	case '9' :
		shape[5]->moveRight();
		break;
	case '6' :
		shape[5]->turnRight();
		break;*/
	case '1':
		//case 1 prints out the position of each of the missiles and the warbird
		shape[5]->printPos();
		shape[8]->printPos();
		shape[9]->printPos();
		break;
	/*case '3':
		shape[5]->rollRight();
		break;
	case '2':
		if(mod != 0)
			shape[5]->turnDown();
		else
			shape[5]->moveBackward();
		break;
	case '5' :
		shape[5]->moveUp();
		break;
	case '0' :
		shape[5]->moveDown();
		break;*/
	case 033 : case 'q' :  case 'Q' : 
		exit(EXIT_SUCCESS); 
		break;
	case 'v' : case 'V' :
		switch(viewCase) {
		case 'f' : viewCase = 't'; break;
		case 't' : viewCase = 'b'; break;
		case 'b' : viewCase = 'f'; break;
		default : viewCase = 'f'; break;
		}
		break;
	case 'p' : case 'P' :
		switch(viewCase) {
		case 'u' : viewCase = 'd'; break;
		case 'd' : viewCase = 'u'; break;
		default : viewCase = 'u'; break;
		}
		break;
	case 'w' : case 'W' :
		switch(warpCase) {
		case 'u' : warpCase = 'd'; shape[5]->warpToPlanet(shape[1]->getTranslationMat(), shape[1]->getRotationMat());
			break;
		case 'd' : warpCase = 'u'; shape[5]->warpToPlanet(shape[2]->getTranslationMat(), shape[2]->getRotationMat());
			break;
		default : warpCase = 'u'; break;
		}
		break;
	case 'f' : case 'F' : //Fire missile
		if(shape[5]->missiles > 0 && !shape[8]->inFlight && !shape[5]->isDead) {
			printf("FIRE!!!");
			shape[8]->traveled = 0;
			shape[8]->fire(shape[5]->getRotationMat(), shape[5]->getTranslationMat());
			shape[5]->missiles--;
		} else if(shape[5]->isDead) {
			printf("Ship is dead\n");
		} else if(shape[5]->missiles == 0) {
			printf("No more missiles!");
		} else {
			printf("Missile is already in Flight!");
		}
		break;
	case 't' : case 'T' : //Change Time Quantum
		switch(timerDelay) {
		case ACE: timerDelay = PILOT; break;
		case PILOT: timerDelay = TRAINEE; break;
		case TRAINEE: timerDelay = DEBUG; break;
		case DEBUG: timerDelay = ACE; break;
		}
		break;
	case 'g' : case 'G' : //Toggle Gravity
		gravity = !gravity;
		break;
	case 'r' : case 'R' : //Reset missiles and Game
		shape[5]->missiles = 10;
		shape[6]->missiles = 5;
		shape[7]->missiles = 5;
		shape[5]->setTranslationMat(glm::translate(glm::mat4(), glm::vec3(500.0f,0,500.0f)));
		shape[5]->isDead = shape[6]->isDead = shape[7]->isDead = false;
		break;


	}

	updateTitle();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("465 Ruber Solar System {v, p, w, f, t} : Front View");
	// initialize and verify glew
	glewExperimental = GL_TRUE;  // needed my home system 
	GLenum err = glewInit();  
	if (GLEW_OK != err) 
		printf("GLEW Error: %s \n", glewGetErrorString(err));      
	else {
		printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
		printf("OpenGL %s, GLSL %s\n", 
			glGetString(GL_VERSION),
			glGetString(GL_SHADING_LANGUAGE_VERSION));
	}
	// initialize scene
	init();
	// set glut callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(process_SHIFT_ALT_CTRL);
	glutIdleFunc(animate);
	glutTimerFunc(timerDelay, intervalTimer, 1);
	glutMainLoop();
	printf("done\n");
	return 0;
}