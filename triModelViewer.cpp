/*

TriModelViewer.cpp

465 utility include files:  shader465.h, triModel465.h  
Shaders:  simpleVertex.glsl  amd simpleFragment.glsl

C OpenGL Core 3.3 example that loads cube.tri model file
and rotates the cube.

User commands:
'a' switch between interval timer (25 fps) and idle timer (faster?)
'f' view from front (0, 0, 3) looking at origin
't' view from above (0, 3, 3) looking at origin
'b' view from below (0, -3, 0) looking at origin
'o' orthographic projection (note cube doesn't render as cube)
'p' perspective projection (aspect ration renders a cube)

Current state is displayed in the window title.
 
The cube2.tri model uses 12 triangular surfaces to display 6 
triangluated "quad" surfaces.  So, there are 36 = (12 * 3) vertices.

The triModel465.h utility creates colors, and normals for ever vertex.
However the simple*.glsl shaders do not use the normal. 
Since the same color is created for every vertex of every surface,
the object is rendered with flat shading.  

The *.tri model's surfaces must be triangles -- there can be no
co-linear "triangle" edges -- triModel465.h will abort in such 
cases.

Viewer is designed to scale to bounding radius of 100.

You must edit the values for the modelFile and for nVertices.

Mike Barnes
9/10/2013
*/

# define __Windows__ // define your target operating system
# include "../includes465/include465.h"

// Edit the next two lines to view your model.
char * modelFile = "rocket.tri";   // name of tri model file
const GLuint nVertices = 144 * 3;  // 3 vertices per line of model file 
 
float boundingRadius;  // modelFile's bounding radius
// display state and "state strings" for title display

// window title strings
char baseStr[50] =    "465 TriModelViewer {a, o, p, f, t, b} : ";
char projectStr[15] = " perspective";
char viewStr[15] =    " front view";
char fpsStr[15], timerStr[20] = " interval timer";
char titleStr [100]; 
GLuint vao;  // VertexArrayObject
GLuint shaderProgram; 
char * vertexShaderFile = "viewVertex.glsl";
char * fragmentShaderFile = "viewFragment.glsl";
GLuint modelView, projection;  // ModelViewProjection hangle
glm::mat4 identity(1.0f); 
glm::mat4 rotationMatrix;             // set in init() and spin()
glm::mat4 scaleMatrix;                // set in init()
glm::mat4 projectionMatrix;           // set in reshape() and keyboard()
glm::mat4 modelViewMatrix;            // set in display()

// vectors and values for lookAt and view 
glm::vec3 eye, at, up;
float eyeDistance = 350.0f;
GLfloat aspectRatio;   // for projection matrix

// vectors for "model"
glm::vec4 vertex[nVertices];
glm::vec3 normal[nVertices];
glm::vec4 diffuseColor[nVertices];

// rotation variables
GLfloat rotateRadian = 0.1f;

int timerDelay = 40, frameCount = 0, intervalTimerCalls = 0;
bool idleTimerFlag = true;  // interval or idle timer ?


void init (void) {
  printf("init:  sizeof glm::vec4 %d point %d diffuseColor %d normal %d \n", 
    sizeof(glm::vec4), sizeof(vertex), sizeof(diffuseColor), sizeof(normal));

  // initialize matrices and variables
  rotationMatrix = glm::mat4();
  scaleMatrix = glm::mat4();

  boundingRadius = loadTriModel(modelFile, nVertices, vertex, diffuseColor, normal);
  if (boundingRadius == -1.0f) {
    printf("loadTriModel error:  returned -1.0f \n");
    exit(1); }
    else {
      float size = 100.0f / boundingRadius;
      scaleMatrix = glm::scale(identity, glm::vec3(size));
      printf("loaded %s model with %7.2f bounding radius,  scale factor of %5.2f \n", modelFile, boundingRadius, size);
      }

  shaderProgram = loadShaders(vertexShaderFile,fragmentShaderFile);
  glUseProgram(shaderProgram);

  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  // Create and initialize a buffer object
  GLuint buffer;
  glGenBuffers( 1, &buffer );
  glBindBuffer( GL_ARRAY_BUFFER, buffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof(vertex) + sizeof(diffuseColor) + sizeof(normal), NULL, GL_STATIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertex), sizeof(diffuseColor), diffuseColor );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(vertex) + sizeof(diffuseColor), sizeof(normal), normal );


  // set up vertex arrays (after shaders are loaded)
  GLuint vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

  GLuint vColor = glGetAttribLocation( shaderProgram, "vColor" );
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertex)) );

  GLuint vNormal = glGetAttribLocation( shaderProgram, "vNormal" );
  glEnableVertexAttribArray( vNormal);
  glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertex) + sizeof(diffuseColor)) );

  modelView = glGetUniformLocation(shaderProgram, "ModelView");
  projection = glGetUniformLocation(shaderProgram, "Projection");
  
  // initially use a front view
  eye = glm::vec3(0.0f, 0.0f, eyeDistance);   // eye is eyeDistance "out of screen" from origin
  at  = glm::vec3(0.0f, 0.0f,   0.0f);        // looking at origin
  up  = glm::vec3(0.0f, 1.0f,   0.0f);   // camera'a up vector
  
// set render state values
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
  }

void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  aspectRatio = (GLfloat) width /  (GLfloat) height;
  // initially use a perspective projection
  projectionMatrix = glm::perspective(glm::radians(60.0f), aspectRatio, 1.0f, 10000.0f); 
  strcpy(projectStr, " perspective");
  printf("reshape: width = %4d height = %4d aspect = %5.2f \n", width, height, aspectRatio);
  }

// update and display animation state in window title
void updateTitle() {
  strcpy(titleStr, baseStr);
  strcat(titleStr, projectStr);
  strcat(titleStr, viewStr);
  strcat(titleStr, timerStr);
  strcat(titleStr, fpsStr);
  // printf("title string = %s \n", titleStr);
  glutSetWindowTitle( titleStr);
  }

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // update model view projection matrix
  modelViewMatrix = glm::lookAt(eye, at, up) * rotationMatrix * scaleMatrix;
  glUniformMatrix4fv(modelView, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
  glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
  // draw 
  glDrawArrays(GL_TRIANGLES, 0, nVertices);
  glutSwapBuffers();
  frameCount++;
  }

// for use with Idle and intervalTimer functions 
// to set rotation
void spin(void){
  rotationMatrix = glm::rotate(rotationMatrix, rotateRadian, glm::vec3(0, 1, 0));
  glutPostRedisplay();
  }

// Estimate FPS, use for fixed interval timer driven animation
void intervalTimer (int i) { 
   glutTimerFunc(timerDelay, intervalTimer, 1);
   // compute frames / second
   intervalTimerCalls++;
   if (intervalTimerCalls * timerDelay >= 1000) {
      sprintf(fpsStr, " fps %4d", frameCount );
      intervalTimerCalls = frameCount = 0;
      updateTitle();
      }      
   if (! idleTimerFlag) spin();  // fixed interval timer
  }

void keyboard (unsigned char key, int x, int y) {
  switch(key) {
    case 033 : case 'q' :  case 'Q' : exit(EXIT_SUCCESS); break;
    case 'a' : case 'A' :  // change animation timer
      // printf("%s   %s\n", timerStr, fpsStr);
      if (idleTimerFlag) { // switch to interval timer  
         glutIdleFunc(NULL);
         strcpy(timerStr, " interval timer");  
         idleTimerFlag = false;  
         }         
      else   {         // switch to idle timer
         glutIdleFunc(spin); 
         strcpy(timerStr, " idle timer");
         idleTimerFlag = true;
         }
      break;
    case 'b' : case 'B' :  // bottom view
        eye = glm::vec3(0.0f, -eyeDistance, eyeDistance);   // eye is -eyeDistance "below screen" from origin
        at  = glm::vec3(0.0f, 0.0f,   0.0f);                // looking at origin
        up  = glm::vec3(0.0f, 1.0f,   0.0f);                // camera'a up vector
        strcpy(viewStr, " bottom view"); break;
    case 'f' : case 'F' :  // front view
        eye = glm::vec3(eyeDistance, 0.0f, eyeDistance);    // eye is eyeDistance "out of screen" from origin
        at  = glm::vec3(  0.0f, 0.0f,   0.0f);              // looking at origin
        up  = glm::vec3(  0.0f, 1.0f,   0.0f);              // camera'a up vector
        strcpy(viewStr, " front view"); break;
    case 't' : case 'T' :  // right view
        eye = glm::vec3(0.0f, eyeDistance, eyeDistance);    // eye is eyeDistance  up from origin
        at  = glm::vec3(0.0f,   0.0f, 0.0f);                // looking at origin
        up  = glm::vec3(0.0f,   1.0f, 0.0f);                // camera's up is looking towards -Z vector
        strcpy(viewStr, " top view"); break;
    case 'p' : case 'P' : // set perspective projection
      projectionMatrix = glm::perspective(glm::radians(60.0f), aspectRatio, 1.0f, 10000.0f); 
      strcpy(projectStr, " perspective"); break;
    case 'o' : case 'O' : // set orthographic perspective projection
      projectionMatrix = glm::ortho(-eyeDistance/2, eyeDistance/2, -eyeDistance/2, eyeDistance/2, 1.0f, 10000.0f);
      strcpy(projectStr, " orthographic"); break;
    }
  updateTitle();
  }
    
int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutInitContextVersion(3, 3);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutCreateWindow("465 CubeTriModel Example {a, o, p, f, t, b} : perspective front view");
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
  glutIdleFunc(spin);
  glutTimerFunc(timerDelay, intervalTimer, 1);
  glutMainLoop();
  printf("done\n");
  return 0;
  }
  
