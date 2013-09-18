/*
triModel465.h

Function to read a *.tri model file exported from AC3D and create
glm::vec4 arrays for vertex, color, and normal surface data.
A vec4 color value and a vec3 normal value is computed for 
every vertex (not surface).

The 3 glm::vec? parameters are empty on call and with values
on return.  

Function's 3 assumptions:
1.  the *.tri model is centered at (0,0,0).
2.  the *.tri model's vertices have been optimized ( Object | OptimizeVertices ... )
3.  the *.tri model's surfaces have been optimized ( Object | OptimizeSurfaces ... )

Function prints various error messages, with error returns -1.0f
Function returns the bounding radius of the model with valid model file.

Mike Barnes
8/17/2013
*/


float loadTriModel(char * fileName, int nVertices, glm::vec4 vertex[], glm::vec4 color[], glm::vec3 normal[]  ) {
  const int X = 0, Y = 1, Z = 2;
  FILE * fileIn;
  glm::vec3 point[3];   // 3 vertices of a triangle
  glm::vec4 surfaceColor; 
  bool done;
  float coord[3][3];
  float maxAxes[3] = {0.0f, 0.0f, 0.0f}; // maximum lenght of x, y, and z from center
  unsigned int c;
  unsigned int r, g, b;
  int count = 0, vertexCount = 0, normalCount = 0, colorCount = 0;

  fileIn = fopen(fileName, "r");
  if (fileIn != NULL) {
    while (!feof(fileIn)) {
        done = false;
        for(int i = 0; i < 3; i++)
          for(int j = 0; j < 3; j++) {
            coord[i][j] = -99999.9f;
            if ( fscanf(fileIn, "%f", &coord[i][j]) == EOF) { // at EOF
              done = true;
              break; }
          }
        if(!done) {  
          // create vertices and normals
          for(int i = 0; i < 3; i++) {
            point[i] = glm::vec3(coord[i][X], coord[i][Y], coord[i][Z]); 
            vertex[vertexCount] = glm::vec4(point[i].x, point[i].y, point[i].z, 1.0f);
            if (maxAxes[X] < abs(coord[i][X])) maxAxes[X] = abs(coord[i][X]);
            if (maxAxes[Y] < abs(coord[i][Y])) maxAxes[X] = abs(coord[i][Y]);
            if (maxAxes[Z] < abs(coord[i][X])) maxAxes[Z] = abs(coord[i][Z]);
            vertexCount++;  }
          // compute for counter-clockwise vertex winding
          normal[normalCount++] = glm::normalize(glm::cross(point[1] - point[0], point[2] - point[0])); 
          normal[normalCount++] = glm::normalize(glm::cross(point[2] - point[1], point[0] - point[1]));  
          normal[normalCount++] = glm::normalize(glm::cross(point[0] - point[2], point[1] - point[2])); 
          fscanf(fileIn, "%x", &c);
          r = c;
          g = c;
          b = c;
          r = r >> 16;
          g = g << 16;
          g = g >> 24;   
          b = b << 24;
          b = b >> 24;
          // make surfaceColor float r,g,b values 0..1
          surfaceColor = glm::vec4(r / 255.0f, g / 255.0f,  b / 255.0f, 1.0f);
          for(int i = 0; i < 3; i++) color[colorCount++] = surfaceColor;
          count++;
          }  // ! done
        // create vertexs, normal, color 
      } // while
    } // if
  else {
      printf("loadTriModel error: can't open %s\n", fileName);
      exit(1); 
      }
  if (count != nVertices/3) {
    printf("loadTriModel error count of surfaces != numberVertex:  count %4d != nVertices/3 %4d\n",
      count, nVertices/3);
    return -1.0f; }
  if (vertexCount != nVertices) {
    printf("loadTriModel error count of vertices mismatch:  vertexCount %4d != nVertices %4d\n",
      vertexCount, nVertices);
    return -1.0f; }
  // return bounding radius
  if (maxAxes[X] >= maxAxes[Y] && maxAxes[X] >= maxAxes[Z]) return maxAxes[X];
  if (maxAxes[Y] >= maxAxes[X] && maxAxes[Y] >= maxAxes[Z]) return maxAxes[Y];
  if (maxAxes[Z] >= maxAxes[X] && maxAxes[Z] >= maxAxes[Y]) return maxAxes[Z];
  }
