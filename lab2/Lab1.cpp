#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <GL/glut.h>
#include <GL/gl.h>
#include <math.h>
#include "SDL.h"
#include "MyVector.h"
#include "GameObject.h"
#include <string.h>

using namespace std;

#define FLT_MIN 1.1754E-38F
#define FLT_MAX 1.1754E+38F

SDL_Surface* screen = NULL;
SDL_Surface* subScreen = NULL;

//constants
const int _windowWidth = 1080;
const int _windowHeight = 780;

const unsigned winPoints = 30;
const float gameOverTime = 60.0;
const int totalEnemyCount = 200;

Uint8* _keys;

//Mesh Vars
float cx, cy, cz;
float max_x, max_y, max_z, min_x, min_y, min_z;
float max_extent;

//Mouse Movement Vars
int mouseX = 0;
int mouseY = 0;

//CameraVars
float ang_x = 0.0;
float ang_y = 0.0;
float ang_z = 0.0;
float pos_x = 0.0;
float pos_y = 0.0;
float pos_z = 0.0;
float rightPlane, leftPlane, top, bottom, near, far;

//Game Vars
vector<GameObject> gameObjects;

//DisplayList Vars
GLuint DLid;
GLuint createDL(void);

//FPS Vars
unsigned frame = 0;
unsigned FPStime,timebase=0;
char FPSDisplay [11];
char ObjectDisplay[15];

//Points value
unsigned points = 0;
char PointsDisplay [11];


//seconds elapsed
unsigned seconds = 0;
char SecondsDisplay [11];

//game over 
char GameOverDisplay [11];

//used for delta time
unsigned now = 0;
unsigned then = 0;
unsigned dt = 0;

//used for delta time
unsigned nowObj = 0;
unsigned thenObj = 0;
unsigned dtObj = 0;

//constants
float MOVE_DELTA = -0.002f;
float MOUSE_DELTA = 0.001f;

//enemy count
int eCount = 0;

// used for frucstrum
float lt[3];
float rt[3];

//bool to cull or not
bool cullToggle = false;
bool drawingMiniMap = false;

//GL light vars
GLfloat light_pos[4] = {0, 0, 1.5, 1.0};
GLfloat light_amb[4] = {0.6, 0.6, 0.6, 1.0};
GLfloat light_diff[4] = {0.6, 0.6, 0.6, 1.0};
GLfloat light_spec[4] = {0.8, 0.8, 0.8, 1.0};

GameObject player;

//Materials
typedef struct materialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess[1];
} materialStruct;

materialStruct RedFlat = {
     {0.3, 0.0, 0.0, 1.0},
     {0.9, 0.0, 0.0, 1.0},
     {0.0, 0.0, 0.0, 1.0},
     {0.0}
   };
   
materialStruct GreenFlat = {
     {0.0, 0.3, 0.0, 1.0},
     {0.0, 0.9, 0.0, 1.0},
     {0.0, 0.0, 0.0, 1.0},
     {0.0}
   };
   
materialStruct BlueFlat = {
     {0.0, 0.0, 0.3, 1.0},
     {0.0, 0.0, 0.9, 1.0},
     {0.0, 0.0, 0.0, 1.0},
     {0.0}
   };

/**an example of a simple data structure to store a 4x4 matrix */
GLfloat ProjectionMatrix[4][4] = {
  {1.0, 0.0, 0.0, 0.0},
  {0.0, 1.0, 0.0, 0.0},
  {0.0, 0.0, 1.0, 0.0},
  {0.0, 0.0, 0.0, 1.0}
};

GLfloat ModelViewMatrix[4][4] = {
  {1.0, 0.0, 0.0, 0.0},
  {0.0, 1.0, 0.0, 0.0},
  {0.0, 0.0, 1.0, 0.0},
  {0.0, 0.0, 0.0, 1.0}
};


GLfloat ProjectionTimesModelview[4][4] = {
  {1.0, 0.0, 0.0, 0.0},
  {0.0, 1.0, 0.0, 0.0},
  {0.0, 0.0, 1.0, 0.0},
  {0.0, 0.0, 0.0, 1.0}
};

//TODO define objects to store 1) vertices 2) faces - be sure you understand the file format
typedef struct  vertice{
  float x;
  float y;
  float z;
  float normX;
  float normY;
  float normZ;
} vertice;
typedef struct triangle {
  int one;
  int two;
  int three;
  float normX;
  float normY;
  float normZ;
  float centX;
  float centY;
  float centZ;
} triangle;
vertice center;
vector<vertice> verts;
vector<triangle> faces;
MyVector myVects;
//forward declarations of functions
void readLine(char* str);
void readStream(istream& is);


void setMaterial(materialStruct materials) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, materials.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, materials.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, materials.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, materials.shininess);
}

//open the file for reading
void ReadFile(char* filename) {
  
  printf("Reading coordinates from %s\n", filename);
  
  ifstream in_f(filename);
  if (!in_f)  {
    printf("Could not open file %s\n", filename);
  } else {
    readStream(in_f);
  }
  
}

void faceNorms()
{
   for(int index = 0; index < faces.size();index++)
   {
      faces[index].normX = (verts[faces[index].three].x + verts[faces[index].two].x + verts[faces[index].one].x);
      faces[index].normY = (verts[faces[index].three].y + verts[faces[index].two].y + verts[faces[index].one].y);
      faces[index].normZ = (verts[faces[index].three].z + verts[faces[index].two].z + verts[faces[index].one].z);
      
      float mag = sqrt(pow(faces[index].normX,2)+pow(faces[index].normY,2)+pow(faces[index].normZ,2));
      faces[index].normX /= mag;
      faces[index].normY /= mag;
      faces[index].normZ /= mag;
   }
}

void set_default_lt_rt()
{
   lt[0] = pos_x - 5 * sin(ang_y - M_PI / 6.0f);
   lt[1] = 9.0f;
   lt[2] = pos_z - 5 * cos(ang_y - M_PI / 6.0f);

   rt[0] = pos_x - 5 * sin(ang_y + M_PI / 6.0f);
   rt[1] = 9.0f;
   rt[2] = pos_z - 5 * cos(ang_y + M_PI / 6.0f);
}

//process the input stream from the file
void readStream(istream& is)
{
  char str[256];
  for (;is;) {
    is >> ws;
    is.get(str,sizeof(str));
    if (!is) break;
    is.ignore(9999,'\n');
    readLine(str);
  }
}

//process each line of input save vertices and faces appropriately
void readLine(char* str) {
  
  int indx = 0, vi;
  float x, y, z;
  float r, g, b;
  int mat;
  
  if (str[0]=='#') return;
  //read a vertex or face
  if (str[0]=='V' && !strncmp(str,"Vertex ",7)) {

    if (sscanf(str,"Vertex %d %g %g %g",&vi,&x,&y,&z) !=4)
    {
      printf("an error occurred in reading vertices\n");
#ifdef _DEBUG
      exit(EXIT_FAILURE);
#endif
    }
    
    //TODO allocate an object to store the vertex or face 
    //store the vertex in a collection
    vertice tempVert;
    tempVert.x = x;
    tempVert.y = y;
    tempVert.z = z;
    tempVert.normX = 0;
    tempVert.normY = 0;
    tempVert.normZ = 0;
    verts.push_back(tempVert);   
	  
    //This code is house keeping to display in center of the scene
    cx += x;
    cy += y;
    cz += z;
    if (x > max_x) max_x = x; if (x < min_x) min_x = x;
    if (y > max_y) max_y = y; if (y < min_y) min_y = y;
    if (z > max_z) max_z = z; if (z < min_z) min_z = z;
  } else if (str[0]=='F' && !strncmp(str,"Face ",5)) {
       triangle tempTri;
	  //TODO allocate an object to store the vertex or face 
	
    char* s=str+4;
    int fi=-1;
    for (int t_i = 0;;t_i++) {
      while (*s && isspace(*s)) s++;
      //if we reach the end of the line break out of the loop
      if (!*s) break;
      //save the position of the current character
      char* beg=s;
      //advance to next space
      while (*s && isdigit(*s)) s++;
      //covert the character to an integer
      int j=atoi(beg);
      //the first number we encounter will be the face index, don't store it
      if (fi<0) { fi=j; continue; }
      //otherwise process the digit we've grabbed in j as a vertex index
      //the first number will be the face id the following are vertex ids
		if (t_i == 1){ 
                   tempTri.one = j-1;
        //TODO store the first vertex in your face object
		}else if (t_i == 2){
                 tempTri.two = j -1 ;
       //TODO store the second vertex in your face object
		}else if (t_i == 3)
                   tempTri.three = j-1;
        //TODO store the third vertex in your face object
      //if there is more data to process break out
      if (*s =='{') break;
    }
    //possibly process colors if the mesh has colors
    if (*s && *s =='{'){
      char *s1 = s+1;
      cout << "trying to parse color " << !strncmp(s1,"rgb",3) << endl;
      //if we're reading off a color
      if (!strncmp(s1,"rgb=",4)) {
        //grab the values of the string
        if (sscanf(s1,"rgb=(%g %g %g) matid=%d",&r,&g,&b,&mat)!=4)
        {
           printf("error during reading rgb values\n");
#ifdef _DEBUG
           exit(EXIT_FAILURE);
#endif
        }
      }
    }
    //store the triangle read in to your face collection
    faces.push_back(tempTri);
    //calcNormals(faces.size()-1);
  }
  faceNorms();
}

//A simple routine that prints the first three vertices and faces to test that you successfully stored the data your data structures....
void printFirstThree() {
   for(int i = 0;i < 3; i++){
      cout << verts[i].x << " " << verts[i].y << " " << verts[i].z << endl;
      cout << "Faces " << verts[faces[i].one].x <<  " " << verts[faces[i].one].y << " " << verts[faces[i].one].z << endl;
   }
      cout << "Should be very first X " << verts[faces[0].one].x << endl;
}


void drawTria(int index) {
  glBegin(GL_TRIANGLES);
  glNormal3f(faces[index].normX,faces[index].normY ,faces[index].normZ);
  
  glVertex3f(verts[faces[index].one].x,verts[faces[index].one].y,verts[faces[index].one].z);
  glVertex3f(verts[faces[index].two].x,verts[faces[index].two].y,verts[faces[index].two].z);
  glVertex3f(verts[faces[index].three].x,verts[faces[index].three].y,verts[faces[index].three].z);
  glEnd();
}

void drawBunny(void){
  for(unsigned int j = 0; j < faces.size(); j++) {
      drawTria(j);
    }
}

GLuint createDL() {
	GLuint bunnyDL;

	bunnyDL = glGenLists(1);

	glNewList(bunnyDL,GL_COMPILE);
	drawBunny();
	glEndList();

	return(bunnyDL);
}

void renderBitmapString(float x, float y, float z, char *string)
{  
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }
}

void drawWireFramePlane()
{
  
  glPushMatrix(); {
    // Draw Grid
    setMaterial(BlueFlat);
    glBegin(GL_LINES); {
      for (float i = -8.0; i <= 8.0f; i += 1) {
        glVertex3f(i, 0, -8.0f);
        glVertex3f(i, 0, 8.0f);
        glVertex3f(-8.0f, 0, i);
        glVertex3f(8.0f, 0, i);
        
        glVertex3f(i, 0, -4.0f);
        glVertex3f(i, 0, 4.0f);
        glVertex3f(-4.0f, 0, i);
        glVertex3f(4.0f, 0, i);
      }
    }
    glEnd();
  }
  glPopMatrix();
}

void setOrthographicProjection() {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save previous matrix which contains the 
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, _windowWidth, 0, _windowHeight);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -_windowHeight, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() {
	// set the current matrix to GL_PROJECTION
	glMatrixMode(GL_PROJECTION);
	// restore previous settings
	glPopMatrix();
	// get back to GL_MODELVIEW matrix
	glMatrixMode(GL_MODELVIEW);
}

void drawStats()
{
  glPushMatrix();
  
  frame++;
  FPStime=glutGet(GLUT_ELAPSED_TIME);
  
  if (FPStime - timebase > 1000) {
    sprintf(FPSDisplay,"FPS:%4.2f",frame*1000.0/(FPStime-timebase));
    sprintf(PointsDisplay,"H:%d",points);
    timebase = FPStime;		
    frame = 0;
  }
  sprintf(SecondsDisplay,"%.2f sec",((float)seconds)/1000.0);
  sprintf(ObjectDisplay,"# of objs: %d",gameObjects.size());
  glColor3f(1.0, 0.0, 0.0);
  glLoadIdentity();
  setOrthographicProjection();
  if(winPoints <= points){
    sprintf(GameOverDisplay,"You Won!!");
    //printf("Game Over!!\n");
    renderBitmapString(_windowWidth/2-30,_windowHeight/2,0,GameOverDisplay);
    if(gameOverTime+5.0 <= ((float)seconds)/1000.0){
      exit(0);
    }
  }
  else if(gameOverTime <= ((float)seconds)/1000.0){
    sprintf(GameOverDisplay,"Game Over!!");
    //printf("Game Over!!\n");
    renderBitmapString(_windowWidth/2-50,_windowHeight/2,0,GameOverDisplay);
    if(gameOverTime+5.0 <= ((float)seconds)/1000.0){
      exit(0);
    }
  }
  else{
    renderBitmapString(1,15,0,FPSDisplay);
    renderBitmapString(1,35,0,PointsDisplay);
    renderBitmapString(1,55,0,SecondsDisplay);
    renderBitmapString(1,75,0,ObjectDisplay);
  }
  
    
  resetPerspectiveProjection();
  glPopMatrix();
}

void initViewFrustum()
{
   glGetFloatv(GL_PROJECTION_MATRIX, *ProjectionMatrix);
   glGetFloatv(GL_MODELVIEW_MATRIX, *ModelViewMatrix);
   //Do Projection * ModelView
   /*ma00*mb00 + ma01*mb10 + ma02*mb20 + ma03*mb30  	ma00*mb01 + ma01*mb11 + ma02*mb21 + ma03*mb31  	ma00*mb02 + ma01*mb12 + ma02*mb22 + ma03*mb32  	ma00*mb03 + ma01*mb13 + ma02*mb23 + ma03*mb33
   
ma10*mb00 + ma11*mb10 + ma12*mb20 + ma13*mb30 	ma10*mb01 + ma11*mb11 + ma12*mb21 + ma13*mb31 	ma10*mb02 + ma11*mb12 + ma12*mb22 + ma13*mb32 	ma10*mb03 + ma11*mb13 + ma12*mb23 + ma13*mb33

ma20*mb00 + ma21*mb10 + ma22*mb20 + ma23*mb30 	ma20*mb01 + ma21*mb11 + ma22*mb21 + ma23*mb31 	ma20*mb02 + ma21*mb12 + ma22*mb22 + ma23*mb32 	ma20*mb03 + ma21*mb13 + ma22*mb23 + ma23*mb33

ma30*mb00 + ma31*mb10 + ma32*mb20 + ma33*mb30 	ma30*mb01 + ma31*mb11 + ma32*mb21 + ma33*mb31 	ma30*mb02 + ma31*mb12 + ma32*mb22 + ma33*mb32 	ma30*mb03 + ma31*mb13 + ma32*mb23 + ma33*mb33*/
   for(int i = 0; i < 4; i++)
   {
      ProjectionTimesModelview[i][0] = ProjectionMatrix[i][0] * ModelViewMatrix[0][0] + ProjectionMatrix[i][1] * ModelViewMatrix[1][0] + ProjectionMatrix[i][2] * ModelViewMatrix[2][0] + ProjectionMatrix[i][3] * ModelViewMatrix[3][0];    
   }
   for(int i = 0; i < 4; i++)
   {
      ProjectionTimesModelview[i][1] = ProjectionMatrix[i][0] * ModelViewMatrix[0][1] + ProjectionMatrix[i][1] * ModelViewMatrix[1][1] + ProjectionMatrix[i][2] * ModelViewMatrix[2][1] + ProjectionMatrix[i][3] * ModelViewMatrix[3][1]; 
   }
   for(int i = 0; i < 4; i++)
   {
      ProjectionTimesModelview[i][2] = ProjectionMatrix[i][0] * ModelViewMatrix[0][2] + ProjectionMatrix[i][1] * ModelViewMatrix[1][2] + ProjectionMatrix[i][2] * ModelViewMatrix[2][2] + ProjectionMatrix[i][3] * ModelViewMatrix[3][2]; 
   }
   for(int i = 0; i < 4; i++)
   {
      ProjectionTimesModelview[i][3] = ProjectionMatrix[i][0] * ModelViewMatrix[0][3] + ProjectionMatrix[i][1] * ModelViewMatrix[1][3] + ProjectionMatrix[i][2] * ModelViewMatrix[2][3] + ProjectionMatrix[i][3] * ModelViewMatrix[3][3]; 
   }
   //Test Against Known Point then normalize!!
   //float right, left, top, bottom, near, far;

   float zVal = sqrt(pow(1,2.0f) + pow(1,2.0f));
   float pointX = pos_x + sin(ang_x);
   float pointY = pos_y + cos(ang_y);
   rightPlane = (ProjectionTimesModelview[3][0]-ProjectionTimesModelview[0][0]) * pointX + (ProjectionTimesModelview[3][1]-ProjectionTimesModelview[0][1]) * pointY + (ProjectionTimesModelview[3][2]-ProjectionTimesModelview[0][2]) * zVal + (ProjectionTimesModelview[3][3]-ProjectionTimesModelview[0][3]);
   
   leftPlane = (ProjectionTimesModelview[0][0]+ProjectionTimesModelview[3][0]) * pointX + (ProjectionTimesModelview[0][1]+ProjectionTimesModelview[3][1]) * pointY + (ProjectionTimesModelview[0][2]+ProjectionTimesModelview[3][2]) * zVal + (ProjectionTimesModelview[0][3]+ProjectionTimesModelview[3][3]);
   
   top = (ProjectionTimesModelview[1][0]+ProjectionTimesModelview[3][0]) * pointX + (ProjectionTimesModelview[1][1]+ProjectionTimesModelview[3][1]) * pointY + (ProjectionTimesModelview[1][2]+ProjectionTimesModelview[3][2]) * zVal + (ProjectionTimesModelview[1][3]+ProjectionTimesModelview[3][3]);
   
   bottom = (ProjectionTimesModelview[3][0]-ProjectionTimesModelview[1][0]) * pointX + (ProjectionTimesModelview[3][1]-ProjectionTimesModelview[1][1]) * pointY + (ProjectionTimesModelview[3][2]-ProjectionTimesModelview[1][2]) * zVal + (ProjectionTimesModelview[3][3]-ProjectionTimesModelview[1][3]);
   
   far = (ProjectionTimesModelview[3][0]-ProjectionTimesModelview[2][0]) * pointX + (ProjectionTimesModelview[3][1]-ProjectionTimesModelview[2][1]) * pointY + (ProjectionTimesModelview[3][2]-ProjectionTimesModelview[2][2]) * zVal + (ProjectionTimesModelview[3][3]-ProjectionTimesModelview[2][3]);
   
   near = (ProjectionTimesModelview[2][0]+ProjectionTimesModelview[3][0]) * pointX + (ProjectionTimesModelview[2][1]+ProjectionTimesModelview[3][1]) * pointY + (ProjectionTimesModelview[2][2]+ProjectionTimesModelview[3][2]) * zVal + (ProjectionTimesModelview[2][3]+ProjectionTimesModelview[3][3]);
   
}

int culled(int index)
{
   float rightTestObject, leftTestObject, topTestObject, bottomTestObject, nearTestObject, farTestObject;
   rightTestObject = (ProjectionTimesModelview[3][0]-ProjectionTimesModelview[0][0])/rightPlane * gameObjects[index].boundingBox.lowerLeftX + (ProjectionTimesModelview[3][1]-ProjectionTimesModelview[0][1])/rightPlane * gameObjects[index].boundingBox.lowerLeftY + (ProjectionTimesModelview[3][2]-ProjectionTimesModelview[0][2])/rightPlane * gameObjects[index].boundingBox.lowerLeftZ + (ProjectionTimesModelview[3][3]-ProjectionTimesModelview[0][3])/rightPlane;
   if(rightTestObject < 0)
   {
      return 0;
   }
   leftTestObject = (ProjectionTimesModelview[0][0]+ProjectionTimesModelview[3][0])/leftPlane * gameObjects[index].boundingBox.upperRightX + (ProjectionTimesModelview[0][1]+ProjectionTimesModelview[3][1])/leftPlane * gameObjects[index].boundingBox.upperRightY + (ProjectionTimesModelview[0][2]+ProjectionTimesModelview[3][2])/leftPlane * gameObjects[index].boundingBox.upperRightZ + (ProjectionTimesModelview[0][3]+ProjectionTimesModelview[3][3])/leftPlane;
   if(leftTestObject < 0)
   {
      return 0;
   }
   topTestObject = (ProjectionTimesModelview[1][0]+ProjectionTimesModelview[3][0])/top * gameObjects[index].boundingBox.lowerLeftX + (ProjectionTimesModelview[1][1]+ProjectionTimesModelview[3][1])/top * gameObjects[index].boundingBox.lowerLeftY + (ProjectionTimesModelview[1][2]+ProjectionTimesModelview[3][2])/top * gameObjects[index].boundingBox.lowerLeftZ + (ProjectionTimesModelview[1][3]+ProjectionTimesModelview[3][3])/top;
   if(topTestObject < 0)
   {
      return 0;
   }
   bottomTestObject = (ProjectionTimesModelview[3][0]-ProjectionTimesModelview[1][0])/bottom * gameObjects[index].boundingBox.upperRightX + (ProjectionTimesModelview[3][1]-ProjectionTimesModelview[1][1])/bottom * gameObjects[index].boundingBox.upperRightY + (ProjectionTimesModelview[3][2]-ProjectionTimesModelview[1][2])/bottom * gameObjects[index].boundingBox.upperRightZ + (ProjectionTimesModelview[3][3]-ProjectionTimesModelview[1][3])/bottom;
   if(bottomTestObject < 0)
   {
      return 0;
   }
   nearTestObject = (ProjectionTimesModelview[2][0]+ProjectionTimesModelview[3][0])/near * gameObjects[index].boundingBox.lowerLeftX + (ProjectionTimesModelview[2][1]+ProjectionTimesModelview[3][1])/near * gameObjects[index].boundingBox.lowerLeftY + (ProjectionTimesModelview[2][2]+ProjectionTimesModelview[3][2])/near * gameObjects[index].boundingBox.lowerLeftZ + (ProjectionTimesModelview[2][3]+ProjectionTimesModelview[3][3])/near;
   if(nearTestObject < 0)
   {
      return 0;
   }
   farTestObject = (ProjectionTimesModelview[3][0]-ProjectionTimesModelview[2][0])/far * gameObjects[index].boundingBox.upperRightX + (ProjectionTimesModelview[3][1]-ProjectionTimesModelview[2][1])/far * gameObjects[index].boundingBox.upperRightY + (ProjectionTimesModelview[3][2]-ProjectionTimesModelview[2][2])/far * gameObjects[index].boundingBox.upperRightZ + (ProjectionTimesModelview[3][3]-ProjectionTimesModelview[2][3])/far;
   if(farTestObject < 0)
   {
      return 0;
   }
   return 1;
}

void drawGameObjects()
{
   glPushMatrix();
   for(int i = 0;i < gameObjects.size();i++)
   {//cout << gameObjects.size() << endl;
      glPushMatrix();
      //glColor3f(gameObjects[i].R, gameObjects[i].G, gameObjects[i].B);
      if(gameObjects[i].alive){
         setMaterial(GreenFlat);
      }
      else{
         setMaterial(RedFlat);
      }
      glTranslatef(gameObjects[i].position.endX,gameObjects[i].position.endY,gameObjects[i].position.endZ);
      if (gameObjects[i].id > 9000)
      {
          glPushMatrix();
          setMaterial(BlueFlat);
          glTranslatef(0, 0.28f, 0);
          glutSolidCube(0.5);
          glPopMatrix();
          
      }
      else 
      {
         if(!cullToggle){
            glCallList(DLid);
         }
         else if(culled(i)){
             glCallList(DLid);
         }
         /*else if (gameObjects[i].position.endX >= 0){
            glCallList(DLid);
         }*/
      }
      
      glPopMatrix();
   }
   glPopMatrix();
}

void drawMiniMap()
{
   drawingMiniMap = true;
   glPushMatrix();
      /* Set the viewport to 3/4 of the way across the screen on the bottom.
       * It should take up the bottom right corner of the screen.
       */
      glViewport (_windowWidth*0.75, _windowHeight*0.75, _windowWidth/4, _windowHeight/4);
      //glMatrixMode (GL_PROJECTION);		/* Select The Projection Matrix */
      glLoadIdentity ();							/* Reset The Projection Matrix */
      /* Set Up Ortho Mode To Fit 1/4 The Screen (Size Of A Viewport) */
         //gluOrtho2D(0, GW/2, GH/2, 0);
      //useOrtho();

   glPushMatrix(); {
      glRotatef(90, 1, 0, 0);
      glTranslatef(0, -5.0f, 0);
      

      // Draw a solid sphere that signifies you
      //glColor3f(0, 1, 0);
      glutSolidSphere(0.25f, 5, 5);
      
      
      // Draw actual scene
      glEnable(GL_LIGHTING);
      glPushMatrix(); {
      
         glTranslatef(pos_x, 0, pos_z);
         drawWireFramePlane();
         drawGameObjects();
      }
      glPopMatrix();
      glDisable(GL_LIGHTING);

      // Draw neat blended viewing frustum
      glEnable(GL_BLEND);
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_ALWAYS, 0.01);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glPushMatrix(); {
         glColor4f(1, 0, 0, 0.5f);

         glBegin(GL_TRIANGLES);
         glVertex3f(pos_x,pos_y,pos_z);
         glVertex3f(lt[0], lt[1], lt[2]);
         glVertex3f(rt[0], rt[1], rt[2]);
         glEnd();
      }
      glPopMatrix();
      glDisable(GL_BLEND);
      glDisable(GL_ALPHA_TEST);

   }
   glPopMatrix();
   glEnable(GL_LIGHTING);
   SDL_GL_SwapBuffers();
   drawingMiniMap = false;
}

void setCameraMode(int width, int height) // reshape the window when it's moved or resized
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
  gluPerspective(90,(float)width/(float)height,0.1,15);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, width, height);

}

GLvoid DrawScene(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   initViewFrustum();
   glRotatef(360.0f - (ang_x * (180.0f / M_PI)), 1, 0, 0);
   glRotatef(360.0f - (ang_y * (180.0f / M_PI)), 0, 1, 0);
   glTranslatef(-pos_x, -pos_y, -pos_z);
   // Loop to draw the main viewport and the minimap
   for (int loop = 0; loop < 2; loop++)
      {
      // Draw the main screen
      if(loop == 0)
      {
         drawWireFramePlane();
         drawGameObjects();
         drawStats();
      }
      if(loop == 1)
      {
         drawMiniMap();
         setCameraMode(_windowWidth, _windowHeight);
      }
   }
	SDL_GL_SwapBuffers();
}

GLboolean CheckKeys(int dt)
{
  bool moved = false;
  if (_keys[SDLK_ESCAPE])
  {
    return true;
  }
    
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  if (_keys[SDLK_s]) {
    x += -MOVE_DELTA * sin(ang_y);
    z += -MOVE_DELTA * cos(ang_y);
    y += MOVE_DELTA * sin(ang_x);
    moved = true;
  }

  if (_keys[SDLK_w]) {
    x += MOVE_DELTA * sin(ang_y);
    z += MOVE_DELTA * cos(ang_y);
    y += -MOVE_DELTA * sin(ang_x);
    moved = true;
  }

  if (_keys[SDLK_d]) {
    x += -MOVE_DELTA * cos(ang_y);
    z += MOVE_DELTA * sin(ang_y);
    moved = true;
  }

  if (_keys[SDLK_a]) {
    x += MOVE_DELTA * cos(ang_y);
    z += -MOVE_DELTA * sin(ang_y);
    moved = true;
  }

  if(_keys[SDLK_KP_PLUS]){
    MOVE_DELTA += -0.0002f;
  }
  
  if(_keys[SDLK_KP_MINUS]){
    MOVE_DELTA -= -0.0002f;
  }
  
  if(moved)
  {
    //increment the amount of (speed * time) = distance
    pos_x += x * dt;
    pos_y += y * dt;
    pos_z += z * dt;
    
    player.position.endX = pos_x;
    player.position.endY = pos_y;
    player.position.endZ = pos_z;

    if (pos_y < 0.0f)
    {
      pos_y = 0.0f;
    }
  }
  
  //check for collison with game objects
  int objIndex = player.collidingWithObjects(gameObjects);
  while(objIndex != -1 && gameObjects[objIndex].id <= 9000)
  {
    cout << "Player collided with #" << objIndex << "!" << endl;
    //gameObjects.erase (gameObjects.begin()+objIndex);
    gameObjects[objIndex].alive = false;
    gameObjects[objIndex].R = 0.5;
    gameObjects[objIndex].B = 0.0;
    
    points++;
    objIndex = player.collidingWithObjects(gameObjects);
  }
  
  if(_keys[SDLK_HOME]){
    pos_x = 0;
    pos_y = 0;
    pos_z = 0;
    ang_x = 0;
    ang_y = 0;
    ang_z = 0;
  }
  
  set_default_lt_rt();
  return false;
}

float randWrap(float min, float max)
{
  return min+((max-min)*rand()/(RAND_MAX));
}

Uint32 spawnGameObj(Uint32 interval, void *param)
{
  if(gameObjects.size() < totalEnemyCount)
  {
     gameObjects.push_back(GameObject(eCount,MyVector(0.0f,0.0f,0.0f,randWrap(-4.0,4.0),-.03f,randWrap(-4.0,4.0)), 
       MyVector(0.0f,0.0f,0.0f,randWrap(-4.0,4.0),0.0,randWrap(-4.0,4.0)), randWrap(0.001,0.004),-.07f,0.f,-.07f,.07f,.17f,.07f));
     eCount++;
  }
  return interval;
}

Uint32 gameObjStep(Uint32 interval, void *param)
{
  nowObj = glutGet(GLUT_ELAPSED_TIME);
  dtObj = nowObj - thenObj;
  for(int i = 0; i <  gameObjects.size();i++)
  {
     gameObjects[i].step(dtObj,&gameObjects);
  }
  thenObj = glutGet(GLUT_ELAPSED_TIME);  
  return interval;
}

void pos_light() {
  //set the light's position
  glMatrixMode(GL_MODELVIEW);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  glLightfv(GL_LIGHT1, GL_POSITION, light_pos);
  glLightfv(GL_LIGHT2, GL_POSITION, light_pos);
  glLightfv(GL_LIGHT3, GL_POSITION, light_pos);
  
}

void glInit()
{
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);
   glEnable(GL_LIGHT0);
   //set up the diffuse, ambient and specular components for the light
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);
   //specify our lighting model as 1 normal per face
   glShadeModel(GL_FLAT);
   glEnable(GL_LIGHTING);
   pos_light();
}

int main(int argc, char *argv[])
{
  //Initialization
  glutInit(&argc, argv);
  max_x = max_y = max_z = FLT_MIN;
  min_x = min_y = min_z = FLT_MAX;
  cx =cy = cz = 0;
  max_extent = 1.0;
  player = GameObject(-1, MyVector(0.f,0.f,0.f,0.f,0.f,0.f), MyVector(0.f,0.f,0.f,0.f,0.f,0.f),0,-0.1f,-0.1f,0.f,0.1f,0.1f,0.1f);
  //player.setBoundingBox(-0.1, -0.1, 0, 0.1, 0.1, 0.1);
  
  //SDL INITIALIZATIONS
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "Unable to initialize SDL: " << SDL_GetError() << endl;
		exit(1);
	}

   screen = SDL_SetVideoMode(_windowWidth, _windowHeight, 0, SDL_OPENGL );
   //subScreen =  SDL_SetVideoMode(800, 600, 0, SDL_OPENGL); 
	if (screen == NULL)
	{
		cout << "Unable to create OpenGL scene: " << SDL_GetError() << endl;
		exit(2);
	}
	glClearColor( 1.0, 1.0 ,1.0, 1);
	void *param;
	//TIMER CODE
	srand(time(NULL));
	SDL_Init(SDL_INIT_TIMER);
   SDL_AddTimer(Uint32 (100), spawnGameObj, param);
   SDL_AddTimer(Uint32 (10), gameObjStep, param);
   
   gameObjects.push_back(GameObject(9001,MyVector(0.0f,0.0f,0.0f,randWrap(-8.0,8.0),-.03f,randWrap(-8.0,8.0)), 
      MyVector(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f), 0,-.17f,0.f,-.25f,.25f,.25f,.25f));
      
   gameObjects.push_back(GameObject(9002,MyVector(0.0f,0.0f,0.0f,randWrap(-8.0,8.0),-.03f,randWrap(-8.0,8.0)), 
      MyVector(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f), 0,-.17f,0.f,-.25f,.25f,.25f,.25f));
      
   gameObjects.push_back(GameObject(9003,MyVector(0.0f,0.0f,0.0f,randWrap(-8.0,8.0),-.03f,randWrap(-8.0,8.0)), 
      MyVector(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f), 0,-.17f,0.f,-.25f,.25f,.25f,.25f));
      
   gameObjects.push_back(GameObject(9004,MyVector(0.0f,0.0f,0.0f,randWrap(-8.0,8.0),-.03f,randWrap(-8.0,8.0)), 
      MyVector(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f), 0,-.17f,0.f,-.25f,.25f,.25f,.25f));
      
   gameObjects.push_back(GameObject(9005,MyVector(0.0f,0.0f,0.0f,randWrap(-8.0,8.0),-.03f,randWrap(-8.0,8.0)), 
      MyVector(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f), 0,-.17f,0.f,-.25f,.25f,.25f,.25f));
   
   
   //(int _id, MyVector _position, MyVector _direction, float _velocity,float llx, float lly, float llz, float urx, float ury, float urz)
   
   //ENDTIMER CODE
	setCameraMode(_windowWidth, _windowHeight);
	
  //////////////////////////////Mesh Initialize Code//////
  //make sure a file to read is specified
  if (argc > 1) {
    cout << "file " << argv[1] << endl;
    //read-in the mesh file specified
    ReadFile(argv[1]);
    //only for debugging
    printFirstThree();

    //once the file is parsed find out the maximum extent to center and scale mesh
    max_extent = max_x - min_x;
    if (max_y - min_y > max_extent) max_extent = max_y - min_y;
    cout << "max_extent " << max_extent << " max_x " << max_x << " min_x " << min_x << endl;
    cout << "max_y " << max_y << " min_y " << min_y << " max_z " << max_z << " min_z " << min_z << endl;

    center.x = 0;
    center.y = 0;
    center.z = 1;
    //TODO divide by the number of vertices you read in!!!
    center.x = cx/verts.size();
    center.y = cy/verts.size();
    center.z = cz/verts.size();
    cout << "center " << center.x << " " << center.y << " " << center.z << endl;
    cout << "scale by " << 1.0/(float)max_extent << endl;
  }
  else 
  {
    cout << "format is: meshparser filename" << endl;
  }
  //////////////////////////////End Mesh Code////////////
   
	// Set window title
  SDL_WM_SetCaption("Lab1", 0);
  SDL_ShowCursor(SDL_DISABLE);
  then = glutGet(GLUT_ELAPSED_TIME);
  _keys = SDL_GetKeyState(NULL);





  now = glutGet(GLUT_ELAPSED_TIME);
  nowObj = glutGet(GLUT_ELAPSED_TIME);
  
  dt = now - then;
  dtObj = nowObj - thenObj;

  int running = 1;
  DLid = createDL();
  
  glInit();
  while (running)
  {
    DrawScene();
    SDL_Event event;
    float mouseButtonDown = 0;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT){
        running = 0;
      }
      if(event.type == SDL_MOUSEMOTION){
        mouseX = event.motion.xrel;
        mouseY = event.motion.yrel;
        ang_y -= mouseX * MOUSE_DELTA;
        ang_x -= mouseY * MOUSE_DELTA;
        if(ang_x <= -(M_PI/2.0))
        {
          ang_x = -(M_PI/2.0);
        }
        if(ang_x >= (M_PI/2.0))
        {
          ang_x = (M_PI/2.0);
        }
        SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
        SDL_WarpMouse(_windowWidth/2, _windowHeight/2);
        SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);

      }     
      if(event.type == SDL_MOUSEBUTTONDOWN)
      {
        printf("Mouse button %d pressed at (%d,%d)\n",event.button.button, event.button.x, event.button.y);
        mouseButtonDown = 1;
        cullToggle = !cullToggle;
      }
      if (event.type == SDL_MOUSEBUTTONUP)
      {
        printf("The mouse is up\n");
        mouseButtonDown = 0;
      }

      _keys = SDL_GetKeyState(NULL);
      }
      now = glutGet(GLUT_ELAPSED_TIME);
      dt = now - then;
      seconds += dt;
      if (CheckKeys(dt)){
        running = 0;
      }
      then = glutGet(GLUT_ELAPSED_TIME);
  }

  SDL_Quit();
  return 0;
}
