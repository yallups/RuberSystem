/*
file:  rawTexture.cpp

rawTexture.cpp loads a *.raw image file into a 2D texture and displays
it.  It is a re-write of IndexPyramid (for the geometry), that has a 
texture coordinates (s,t) instead of colors.

The loadRawTexture(...) method expects 3 bytes per pixel (24bpp).
The program loads the texture top-left (1,0) to bottom-right(1,0).
Your image may appear upside down depending on how you saved the *.raw
file.  You might need to change the texture coordinates.

The raw file can be converted from other formats by IrfranView.  To
convert to *.raw file format you will also need to install the
plugins.  http://www.irfanview.net/

The texture's width and height and its file name should be set after
the includes.

int width = ????, height = ?????;    // set texture's width and height here
char * fileName = "?????";           // set texture's file name here

This program has:

int width = 256, height = 363;   // set texture's width and height values here
char * fileName = "therapist.raw";  // set the texture file's name here

You will need the file "therapist.raw" also.  
This is Rene Magritte's painting "The Therapist".
http://en.wikipedia.org/wiki/Rene_Magritte


Mike Barnes
11/5/2013
*/

# define __Windows__ // define your target operating system
# include "includes465/include465.h"
# include "includes465/texture.hpp"  // freeTexture(...), loadRawTexture(...)

int width = 1280, height = 720;   // set texture's width and height values here
char * fileName = "stars_01.raw";  // set the texture file's name here

const GLuint NumVertices = 18;
// display state and "state strings" for title display
char viewStr[25] =    " front view";
char baseStr[60] =    "465 Raw Texture {b, f, t} : ";
char titleStr [75]; 

GLuint vao, vbo, ibo;  // VertexArrayObject, VertexBufferObject, IndexBufferObject
GLuint texture, Texture;  // texture id and shader handle
GLuint vPosition, vTexCoord, vNormal;  // VertexAttribPointers
GLuint shaderProgram; 
char * vertexShaderFile = "textureVertex.glsl";
char * fragmentShaderFile = "textureFragment.glsl";
// GLuint modelView, projection ;  // handles for uniform model - view  and projection matrices
GLuint NormalMatrix, MVP;  // ModelViewProjection handle
glm::mat4 projectionMatrix;     // set in display
glm::mat4 modelViewMatrix;      // set in display()
glm::mat3 normalMatrix;         // set in display()
glm::mat4 modelViewProjectionMatrix;  // set in display()
glm::mat4 rotationMatrix;       // set in init() and spin()

// vectors and values for lookAt
glm::vec3 eye, at, up;

// coordinates for "model"
static const GLfloat point[] = {
  -128.0f, -128.0f,  181.0f, 1.0f,  // 0 front left bottom 
   128.0f, -128.0f,  181.0f, 1.0f,  // 1 front right bottom
   128.0f, -128.0f, -181.0f, 1.0f,  // 2 back right bottom
  -128.0f, -128.0f, -181.0f, 1.0f,  // 3 back left bottom
   0.0f,  128.0f,  0.0f, 1.0f}; // 4 apex
  

// indexes to reuse vertex values
static const unsigned int indices[] = { // 6 faces, 3 vertices per face
  0, 1, 4, // Front Face 
  1, 2, 4,   // Right Face 
  2, 3, 4,  // Back Face 
  3, 0, 4,  // Left Face 
  3, 1, 0,  // Front Bottom Face 
  3, 2, 1}; // Back Bottom Face 
  

// vertex normals averaged from shared surface normals
static GLfloat normal[15]; 

// Texture Coordinates for each vertex
// points * 2 (s, t)
static const GLfloat texCoords[] = {  
  0.0f, 0.0f,     // 0
  1.0f, 0.0f,     // 1
  1.0f, 1.0f,     // 2
  0.0f, 1.0f,     // 3
  0.5f, 0.5f };   // 4 apex


// Given a unitNormal, set the 3 coordinates in the normal array 
void makeNormal(int i, glm::vec3 unitNormal)  {
  normal[i]     = unitNormal.x; 
  normal[i + 1] = unitNormal.y;
  normal[i + 2] = unitNormal.z;
  }


void init (void) {

  // make normals from vertices shared surface normals 
  // bottom normals flipped "up" for top view, as there is no bottom view.
  makeNormal( 0, glm::normalize(glm::vec3(-0.00,  0.30,  0.95) + glm::vec3( 0.00, -1.00,  0.00) + glm::vec3(-0.95,  0.30,  0.00))); // front left bottom
  makeNormal( 3, glm::normalize(glm::vec3(-0.00,  0.30,  0.95) + glm::vec3( 0.00, -1.00,  0.00) + 
                 glm::vec3( 0.00, -1.00,  0.00) + glm::vec3( 0.95,  0.30,  0.00))); // front right bottom
  makeNormal( 6, glm::normalize(glm::vec3( 0.00,  0.30, -0.95) + glm::vec3( 0.00, -1.00,  0.00) + glm::vec3( 0.95,  0.30,  0.00))); // back right bottom
  makeNormal( 9, glm::normalize(glm::vec3( 0.00,  0.30, -0.95) + glm::vec3( 0.00, -1.00,  0.00) + 
                 glm::vec3( 0.00, -1.00,  0.00) + glm::vec3(-0.95,  0.30,  0.00))); // back left bottom
  makeNormal(12, glm::normalize(glm::vec3(-0.00,  0.30,  0.95) + glm::vec3( 0.00, 0.30, -0.95) + 
                                glm::vec3( 0.95,  0.30,  0.00) + glm::vec3(-0.95, 0.30,  0.00))); // apex

 
  shaderProgram = loadShaders(vertexShaderFile,fragmentShaderFile);
  glUseProgram(shaderProgram);

  // set up the indices buffer
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // set up the vertex attributes
  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );

  //  initialize a buffer object
  glGenBuffers( 1, &vbo );
  // glEnableVertexAttribArray(vbo);
  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBufferData( GL_ARRAY_BUFFER, sizeof(point) + sizeof(texCoords) + sizeof(normal), NULL, GL_STATIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point), point );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(point), sizeof(texCoords), texCoords);
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(point) + sizeof(texCoords), sizeof(normal), normal );

  // set up vertex arrays (after shaders are loaded)
  vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glEnableVertexAttribArray( vPosition );

  vTexCoord = glGetAttribLocation( shaderProgram, "vTexCoord" );
  glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point)) );
  glEnableVertexAttribArray( vTexCoord );

  vNormal = glGetAttribLocation( shaderProgram, "vNormal" );
  glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point) + sizeof(texCoords)) );
  glEnableVertexAttribArray( vNormal );

  NormalMatrix = glGetUniformLocation(shaderProgram, "NormalMatrix");
  MVP = glGetUniformLocation(shaderProgram, "MVP");
  
  // load texture
  texture = loadRawTexture( texture, fileName, width, height);
  if (texture != 0) {
      printf("texture file, read, texture generated and bound  \n");
      Texture = glGetUniformLocation(shaderProgram, "Texture"); }
    else  // texture file loaded
      printf("Texture in file %s NOT LOADED !!! \n");
     
  // If you want to see what these values are...
  printf("vbo %3d ibo %3d\n", vbo, ibo);
  printf("locations: vPosition %3d, vTexCoord %3d, vNormal %3d, NormalMatrix %3d, MVP %3d, Texture %3d \n", 
    vPosition, vTexCoord, vNormal, NormalMatrix, MVP, Texture); 

  rotationMatrix = glm::mat4();

// initially use a front view
  eye = glm::vec3(300.0f, 300.0f, 300.0f);   // eye is 300 "out of screen" from origin
  at  = glm::vec3(  0.0f,   0.0f,   0.0f);   // looking at origin
  up  = glm::vec3(  0.0f,   1.0f,   0.0f);   // camera'a up vector

  // set render state values
  glCullFace(GL_BACK);  // show only front faces
  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  }

void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  projectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat) width /  (GLfloat) height, 1.0f, 1000.0f); 
  }

void display(void) {
  // printf("display %s %s \n", projectStr, viewStr);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // update model view projection matrix
  modelViewMatrix = glm::lookAt(eye, at, up) * rotationMatrix;   
  // update for vertex shader
  modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;
  glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
  normalMatrix = glm::mat3(modelViewMatrix);
  glUniformMatrix3fv(NormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

  glBindBuffer( GL_ARRAY_BUFFER, vbo );
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  // draw 
  glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
  glutSwapBuffers();
  }

void keyboard (unsigned char key, int x, int y) {
  switch(key) {
    case 033 : case 'q' :  case 'Q' : exit(EXIT_SUCCESS); break;
    case 'b' : case 'B' :  // bottom view
        eye = glm::vec3( 0.0f,-500.0f, 0.0f);   
        at  = glm::vec3( 0.0f,   0.0f, 0.0f);   
        up  = glm::vec3( 0.0f,   0.0f, 1.0f);  
        strcpy(viewStr, " bottom view"); 
        break;
    case 'f' : case 'F' :  // front view
        eye = glm::vec3( 0.0f, 50.0f, 500.0f);   
        at  = glm::vec3( 0.0f,  0.0f,   0.0f);   
        up  = glm::vec3( 0.0f,  1.0f,   0.0f);  
        strcpy(viewStr, " front view"); 
        break;
    case 't' : case 'T' :  // top view
        eye = glm::vec3(0.0f, 300.0f,  0.0f);   
        at  = glm::vec3(0.0f,   0.0f,  0.0f);   
        up  = glm::vec3(0.0f,   0.0f, -1.0f);   
        strcpy(viewStr, " top view"); 
        break;
    }
  strcpy(titleStr, baseStr);
  strcat(titleStr, viewStr);
  glutSetWindowTitle( titleStr);
  glutPostRedisplay();
  }
    
// for use with Idle and intervalTimer functions 
// to set rotation
void spin (int i) { 
  glutTimerFunc(40, spin, 1);
  rotationMatrix = glm::rotate(rotationMatrix, 0.01f, glm::vec3(0, 1, 0));
  glutPostRedisplay();
  }


// free OpenGL resources
void cleanUp (void) {
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ibo);
  freeTexture(texture);   // in includes465/texture.hpp
  printf("cleaned up\n");
  }
 
int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(400, 350);
  glutInitContextVersion(3, 3);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutCreateWindow("465 Raw Texture {b, f, t} : front view");
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
  glutTimerFunc(40, spin, 1);
  glutCloseFunc(cleanUp);  // freeglut only
  // Note: glutSetOption is only available with freeglut
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,
              GLUT_ACTION_GLUTMAINLOOP_RETURNS);
  glutMainLoop();
  printf("exit main() \n");
  return 0;
  }
  