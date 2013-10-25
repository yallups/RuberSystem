/*
manyCubes.cpp
includes Shape3D.cpp

A modification of CubeTriModel.cpp that uses C and C++.
This file (manyCube.cpp) is a C file -- it defines no classes
but has 1000 instances of Shape3D.

File Shape3D defines a 3D shape that can translate and rotate.
There is only 1 model created from the "Cube2.tri" file.

However 1,000 Shapes3D are drawn as cubes.  
Each has its own position and orientation.
Each cube rotates either orbitally or about its center.

465 utility include files:  shader465.h, triModel465.h  
Shaders:  simpleVertex.glsl  amd simpleFragment.glsl

User commands:
'f' view from front (0, 0, 3) looking at origin
't' view from above (0, 3, 3) looking at origin
'b' view from below (0, -3, 0) looking at origin

Current state is displayed in the window title.

The cube2.tri model uses 12 triangular surfaces to display 12 trianlge surfaces.  
So, there are 36 (3 * 12) vertices.

The triModel465.h utility creates colors, and normals for ever vertex.
However the simple*.glsl shaders do not use the normal. 
Since the same color is created for every vertex of every surface,
the object is rendered with flat shading.

Mike Barnes
9/7/2013
*/


# define __Windows__ // define your target operating system
# include "../includes465/include465.h"
# define __INCLUDES465__   

# include "Shape3D.hpp"

// Shapes
const int nShapes = 6;
Shape3D * shape[nShapes];
// Model for shapes
char * modelFile[] = {"sphere_mr.tri", "sphere_mr.tri", "sphere_mr.tri", "sphere_mr.tri", "sphere_mr.tri", "ship03_color.tri"};// "ruber.tri", "unum.tri", "duo.tri", "moon.tri", "moon.tri"
const GLuint nVerticesSphere = 4900 * 3;  // 3 vertices per line (surface) of model file  
const GLuint nVerticesWarbird = 980 * 3;

char viewCase = 'n';

char * rocketModel = "rocket.tri";  // name of Rocket model file
const GLuint nVerticesRocket = 144 * 3;  // 3 vertices per line (surface) of model file  

float boundingRadius[nShapes];  // modelFile's bounding radius
float boundingRadiusRocket;  // modelFile's bounding radius
int Index =  0;  // global variable indexing into VBO arrays

// display state and "state strings" for title display
// window title strings
char baseStr[50] =    "465 Ruber Solar System {f, t, u, d, w} :";
char fpsStr[15], viewStr[15] =    " front view";
char titleStr [100]; 

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

// vectors for "model"
glm::vec4 vertexSphere[nVerticesSphere];
glm::vec3 normalSphere[nVerticesSphere];
glm::vec4 diffuseColorMaterialSphere[nVerticesSphere];

// vectors for "modelRocket"
glm::vec4 vertexWarbird[nVerticesWarbird];
glm::vec3 normalWarbird[nVerticesWarbird];
glm::vec4 diffuseColorMaterialWarbird[nVerticesWarbird];


// rotation variables
glm::mat4 identity(1.0f); 
glm::mat4 rotation;
int timerDelay = 1000, frameCount = 0;


void init (void) {
	for(int i = 0; i < nShapes; i++) {
		if(i < 5){	   
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
		} else if(i == 5) {
			boundingRadius[i] = loadTriModel(modelFile[i], nVerticesWarbird, vertexWarbird, diffuseColorMaterialWarbird, normalWarbird);
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
		}
	}

	// initially use a front view
	eye = glm::vec3(0.0f, 0.0f, 2000.0f);   // eye is 1000 "out of screen" from origin
	at  = glm::vec3(0.0f, 0.0f,    0.0f);   // looking at origin
	up  = glm::vec3(0.0f, 1.0f,    0.0f);   // camera'a up vector
	viewMatrix = glm::lookAt(eye, at, up);

	// set render state values
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// create shape
	for(int i = 0; i < nShapes; i++) shape[i] = new Shape3D(i);
	printf("%d Shapes created \n", nShapes);
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
	glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3(0, 50.0f, -120.0f));
	warbird = warbird*warbirdrot;
	trans = warbird*trans;

	switch(viewCase) {
	case 'u' :// unum view
		eye = glm::vec3(unum[3].x,  200.0f, unum[3].z);   // eye is 3000 up from origin
		at  = glm::vec3(unum[3].x,   0.0f,  unum[3].z);   // looking at origin  
		up  = glm::vec3(0.0f,        0.0f,      -1.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up); 
		break;
	case 'd' : // duo view
		eye = glm::vec3(duo[3].x,  300.0f, duo[3].z);   // eye is 3000 up from origin
		at  = glm::vec3(duo[3].x,    0.0f, duo[3].z);   // looking at origin  
		up  = glm::vec3(0.0f,        0.0f,    -1.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up);
		break;
	case 'w' :  // warbird view
		eye = glm::vec3(trans[3].x,   trans[3].y,   trans[3].z);   // eye is 3000 up from origin
		at  = glm::vec3(warbird[3].x, warbird[3].y+50.0f, warbird[3].z);   // looking at origin  
		up  = glm::vec3(0.0f,  1.0f,    0.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up); 
		break;
	}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// update model matrix, set MVP, draw
	if(viewCase != 'n')
		updateView();
	for(int i = 0; i < nShapes; i++) { 
		modelMatrix = shape[i]->getModelMatrix(shape[2]->getTranslationMat(), shape[2]->getRotationMat()); 
		glBindVertexArray( vao[i] );
		viewProjectionMatrix = projectionMatrix * viewMatrix; 
		glEnableVertexAttribArray( vPosition[0]);
		glEnableVertexAttribArray( vColor[0]);
		glEnableVertexAttribArray( vNormal[0]);
		glUniformMatrix4fv(Model, 1, GL_FALSE, glm::value_ptr(modelMatrix)); 
		glUniformMatrix4fv(ViewProj, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix)); 
		if(i < 5)
			glDrawArrays(GL_TRIANGLES, 0, nVerticesSphere);
		else if(i==5)
			glDrawArrays(GL_TRIANGLES, 0, nVerticesWarbird);
	}
	glutSwapBuffers();
	frameCount++;

}

// for use with Idle and intervalTimer functions 
// to set rotation
void animate(void){
	frameCount++;
	for(int i = 0; i < nShapes; i++) {
		shape[i] -> update();
	}
	glutPostRedisplay();
}


// Estimate FPS, use fixed interval timer to measure every second
// timerDelay = 1000
void intervalTimer (int i) { 
	glutTimerFunc(timerDelay, intervalTimer, 1);
	// display frames / second
	sprintf(fpsStr, " fps %4d", frameCount );
	frameCount = 0;
	updateTitle();
	animate();
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

	printf("%d, %d, process_SHIFT_ALT_CTRL\n", mod, key);

	switch(key) {
	case GLUT_KEY_UP :
		if(mod != 0)
			shape[5]->turnUp();
		else
			shape[5]->moveForward();
		break;
	case GLUT_KEY_LEFT :
		if(mod == 0)
			shape[5]->turnLeft();
		else
			shape[5]->rollLeft();
		break;
	case GLUT_KEY_RIGHT :
		if(mod == 0)
			shape[5]->turnRight();
		else
			shape[5]->rollRight();
		break;
	case GLUT_KEY_DOWN:
		if(mod != 0)
			shape[5]->turnDown();
		else
			shape[5]->moveBackward();
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

	printf("%d, %d, keyboard\n", mod, key);

	switch(key) {
	case '8' :
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
		break;
	case '1':
		shape[5]->rollLeft();
		break;
	case '3':
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
		break;
	case 033 : case 'q' :  case 'Q' : 
		exit(EXIT_SUCCESS); 
		break;
	case 'f' : case 'F' :  // front view
		viewCase = 'n';
		eye = glm::vec3(0.0f, 0.0f, 4000.0f);   // eye is 4000 "out of screen" from origin
		at  = glm::vec3(0.0f, 0.0f,    0.0f);   // looking at origin
		up  = glm::vec3(0.0f, 1.0f,    0.0f);   // camera'a up vector
		viewMatrix = glm::lookAt(eye, at, up);
		strcpy(viewStr, " front view"); 
		break;
	case 't' : case 'T' :  // top view
		viewCase = 'n';
		eye = glm::vec3(0.0f, 2000.0f,  0.0f);   // eye is 4000 up from origin
		at  = glm::vec3(0.0f,    0.0f,  0.0f);   // looking at origin  
		up  = glm::vec3(0.0f,    0.0f, -1.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up);
		strcpy(viewStr, " top view"); 
		break;
	case 'u' : case 'U' :  // unum view
		viewCase = 'u';
		eye = glm::vec3(unum[3].x,  200.0f, unum[3].z);   // eye is 3000 up from origin
		at  = glm::vec3(unum[3].x,   0.0f,  unum[3].z);   // looking at origin  
		up  = glm::vec3(0.0f,        0.0f,      -1.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up); 
		strcpy(viewStr, " unum view"); 
		break;
	case 'd' : case 'D' :  // duo view
		viewCase = 'd';
		eye = glm::vec3(duo[3].x,  200.0f, duo[3].z);   // eye is 3000 up from origin
		at  = glm::vec3(duo[3].x,    0.0f, duo[3].z);   // looking at origin  
		up  = glm::vec3(0.0f,        0.0f,    -1.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up);
		strcpy(viewStr, " duo view"); 
		break;
	case 'w' : case 'W' :  // warbird view
		viewCase = 'w';
		eye = glm::vec3(warbird[3].x, warbird[3].y+50.0f, warbird[3].z-100.0f);   // eye is 3000 up from origin
		at  = glm::vec3(warbird[3].x, warbird[3].y+0.0f,         warbird[3].z);   // looking at origin  
		up  = glm::vec3(0.0f,  1.0f,    0.0f);   // camera's up is looking towards -Z vector
		viewMatrix = glm::lookAt(eye, at, up); 
		strcpy(viewStr, " warbird view"); 
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
	glutCreateWindow("465 Ruber Solar System {f, t, u, d, w} : front view");
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


