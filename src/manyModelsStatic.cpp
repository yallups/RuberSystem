/*

manyModelsStatic.cpp

465 utility include files:  shader465.h, triModel465.h  

Shaders:  viewVertex.glsl amd viewFragment.glsl
  provide ambient, diffuse, and specular lights
  fixed light position

C OpenGL Core 3.3 example that loads "nModels" *.tri model files 
and displays them in at static locations with a static view.
Demonstrates use of triModel465.h functions for loading *.tri models.

Mike Barnes
10/2/2013
*/

# define __Windows__
# include "../../includes465/include465.h"
# define __INCLUDES465__ 


const int X = 0, Y = 1, Z = 2, START = 0, STOP = 1;
// constants for models:  file names, vertex count, model display size
const int nModels = 4;  // number of models in this scene
char * modelFile [nModels] = {"..//models/sphere.tri", "..//models/sphere.tri", "..//models/sphere.tri", "..//models/cube2.tri"};
float modelBR[nModels];       // model's bounding radius
float scaleValue[nModels];    // model's scaling "size" value
const int nVertices[nModels] = { 264 * 3, 264 * 3, 264 * 3, 12 * 3 };
char * vertexShaderFile   = "../shaders/viewVertex.glsl";      // "simpleVertex.glsl";  // "viewVertex.glsl";
char * fragmentShaderFile = "../shaders/viewFragment.glsl";  // "simpleFragment.glsl";  //"viewFragment.glsl";    
GLuint shaderProgram; 
GLuint VAO[nModels];      // Vertex Array Objects
GLuint buffer[nModels];   // Vertex Buffer Objects

 // Shader handles, matrices, etc
GLuint model, view, project;                  // Model, View, Project GLSL uniform handles
GLuint vPosition[nModels], vColor[nModels], vNormal[nModels];   // vPosition, vColor, vNormal handles for models
// model, view, projection matrices and values to create modelMatrix.
float modelSize[nModels] = { 25.0f, 25.0f, 25.0f, 10.0f };   // size of model
glm::vec3 scale[nModels];       // set in init()
glm::vec3 translate[nModels] = {glm::vec3(0,0,500), glm::vec3(300, 0, -500), glm::vec3(-300, 0, -500), glm::vec3(50, 0, 500)};
glm::mat4 modelMatrix;          // set in display()
glm::mat4 viewMatrix;           // set in init()
glm::mat4 projectionMatrix;     // set in reshape()

// display state and "state strings" for title display
// window title strings
char baseStr[50] = "465 manyCubes Example {f, t, r} : ";
char fpsStr[15], viewStr[15] = "";
char titleStr [100];

// vectors and values for lookAt
glm::vec3 eye, at, up;

// Missile firing status.
boolean missileFired = false;
GLfloat missilePosition = 0.0;

void reshape(int width, int height) {
  float aspectRatio = (float) width / (float) height;
  float FOVY = glm::radians(60.0f);
  glViewport(0, 0, width, height);
  printf("reshape: FOVY = %5.2f, width = %4d height = %4d aspect = %5.2f \n", 
    FOVY, width, height, aspectRatio);
  projectionMatrix = glm::perspective(FOVY, aspectRatio, 1.0f, 100000.0f); 
  }

// Fire a missile.
void fireMissile() {

	missileFired = true;
}

void animate() {

	// TODO Need to get current position then adjust by missilePosition.
	// Stop the missile.
	if (missilePosition > 100) {
		missileFired = false;
	}

	// Update the missile's position.
	if (missileFired) {
		missilePosition -= 10.0;

		translate[3] = glm::vec3(50, 0, missilePosition);  // Index 3 corresponds to the missile.
		printf("\n[Missile position] %f", missilePosition);
	}


}

void intervalTimer (int i) {
	glutTimerFunc(1000/60, intervalTimer, 1);

	animate();

	glutPostRedisplay();
}


void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // update view, projection matrix
  glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(project, 1, GL_FALSE, glm::value_ptr(projectionMatrix));   
  // for each model draw its instances -- from solid[start] to solid[stop]
  for(int m = 0; m < nModels; m++) {
      modelMatrix = glm::translate(glm::mat4(), translate[m]) * 
        glm::scale(glm::mat4(), glm::vec3(scale[m]) );
      glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr( modelMatrix)); 
      glBindVertexArray(VAO[m]);
      glEnableVertexAttribArray( vPosition[m] );
      glEnableVertexAttribArray( vColor[m] );
      glEnableVertexAttribArray( vNormal[m] );
      glDrawArrays(GL_TRIANGLES, 0, nVertices[m] );
  }

  glutSwapBuffers();

  }

// update and display animation state in window title
void updateTitle() {
  strcpy(titleStr, baseStr);
  strcat(titleStr, viewStr);
  strcat(titleStr, fpsStr);
  // printf("title string = %s \n", titleStr);
  glutSetWindowTitle( titleStr);
  }

// Process keyboard commands.
void keyboard (unsigned char key, int x, int y) {
  // Set default distance from camera.
  float distFromCamera = 1000.0f;

  switch(key) {
    case 033 : case 'q' :  case 'Q' : exit(EXIT_SUCCESS); break;
    case 'f' : case 'F' :  // front view
        eye = glm::vec3(0.0f, 0.0f, distFromCamera);   // eye is n-units "out of screen" from origin
        at  = glm::vec3(0.0f, 0.0f,    0.0f);   // looking at origin
        up  = glm::vec3(0.0f, 1.0f,    0.0f);   // camera's up vector
        strcpy(viewStr, " front view");
		break;
    case 'r' : case 'R' :  // bottom view
        eye = glm::vec3(distFromCamera, 0.0f, 0.0f);   // eye is n-units right from origin
        at  = glm::vec3(   0.0f, 0.0f, 0.0f);   // looking at origin
        up  = glm::vec3(   0.0f, 1.0f, 0.0f);   // camera's up vector
        strcpy(viewStr, " right view");
		break;
    case 't' : case 'T' :  // top view
        eye = glm::vec3(0.0f, distFromCamera,  0.0f);   // eye is n-units up from origin
        at  = glm::vec3(0.0f,    0.0f,  0.0f);   // looking at origin  
        up  = glm::vec3(0.0f,    0.0f, -1.0f);   // camera's up is looking towards -Z vector
        strcpy(viewStr, " top view");
		break;
	case 'm' : case 'M' : // Fire missile
		fireMissile(); // Update the missile's position.
		printf("\nMissile fired.");
		break;
	case 'p' : case 'P' : // Reset the missile's starting position.
		missileFired = false;
		translate[3] = glm::vec3(50, 0, 500);
		missilePosition = 0.0;
		printf("\nMissile reset.");
		break;
    }

  viewMatrix = glm::lookAt(eye, at, up);
  updateTitle();

}

// load the shader programs, vertex data from model files, create the solids, set initial view
void init() {
  // load the shader programs
  shaderProgram = loadShaders(vertexShaderFile,fragmentShaderFile);
  glUseProgram(shaderProgram);
  
  // generate VAOs and VBOs
  glGenVertexArrays( nModels, VAO );
  glGenBuffers( nModels, buffer );
  // load the buffers from the model files
  for (int i = 0; i < nModels; i++) {
    modelBR[i] = loadModelBuffer(modelFile[i], nVertices[i], VAO[i], buffer[i], shaderProgram,
      vPosition[i], vColor[i], vNormal[i], "vPosition", "vColor", "vNormal"); 
    // set scale for models given bounding radius  
    scale[i] = glm::vec3( modelSize[i] * 1.0f/modelBR[i] );
    }
    

  model =   glGetUniformLocation(shaderProgram, "Model");
  view =    glGetUniformLocation(shaderProgram, "View");
  project = glGetUniformLocation(shaderProgram, "Project");

  printf("model %d, view %d, project %d\n", model, view, project);

  // Set the initial view.
  //viewMatrix = glm::lookAt(
  //  glm::vec3(50.0f, 50.0f, 200.0f),  // eye position
  //  glm::vec3(0),                   // look at position
  //  glm::vec3(0.0f, 1.0f, 0.0f)); // up vect0r
  keyboard('t', 0, 0);

  // set render state values
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
  }

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitContextVersion(3, 3);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutCreateWindow("465 manyModelsStatic Example");
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
  //glutIdleFunc(animate);
  glutTimerFunc(100, intervalTimer, 0);
  glutMainLoop();
  printf("done\n");
  return 0;
  }
  

