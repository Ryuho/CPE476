#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <GL/glut.h>
#include <GL/gl.h>
#include "SDL.h"
#include "MyVector.h"
#include "GameObject.h"
#include <string.h>

using namespace std;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_DEPTH 8
#define FLT_MIN 1.1754E-38F
#define FLT_MAX 1.1754E+38F

int _windowWidth;
int _windowHeight;
Uint8* _keys;

//Mesh Vars
float cx, cy, cz;
float max_x, max_y, max_z, min_x, min_y, min_z;
float max_extent;

//Mouse Movement Vars
float mouseX;
float mouseY;

//CameraVars
MyVector eyePoint;
MyVector lookAt;
MyVector up;

//Player Movement Vars
float forewardBackwardMovement;
float leftRightMovement;

//Game Vars
vector<GameObject> gameObjects;

//DisplayList Vars
GLuint DLid;
GLuint createDL(void);

//FPS Vars
int frame = 0;
int FPStime,timebase=0;
char FPSDisplay [11];



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
  //faceNorms();
}

//TODO implement a simple routine that prints the first three vertices and faces to test that you successfully stored the data your data structures....
void printFirstThree() {
   for(int i = 0;i < 3; i++){
      cout << verts[i].x << " " << verts[i].y << " " << verts[i].z << endl;
      cout << "Faces " << verts[faces[i].one].x <<  " " << verts[faces[i].one].y << " " << verts[faces[i].one].z << endl;
   }
      cout << "Should be very first X " << verts[faces[0].one].x << endl;
}

//TODO imlement a drawing routine to draw triangles as wireframe
void drawTria(int index) {
    /*if(mode != 6)
    {
       glNormal3f((faces[index].normX/30.0) + faces[index].centX,(faces[index].normY/30.0) + faces[index].centY ,(faces[index].normZ/30.0) + faces[index].centZ);
    }
    if(fillPoly)
    {*/
       glBegin(GL_TRIANGLES);
    /*}
    else
    {
       glBegin(GL_LINE_LOOP);
    }*/
      glColor3f(0.0, 0.0, 0.5);
         glNormal3f(verts[faces[index].one].normX,verts[faces[index].one].normY,verts[faces[index].one].normZ);
         glVertex3f(verts[faces[index].one].x,verts[faces[index].one].y,verts[faces[index].one].z);
         glNormal3f(verts[faces[index].two].normX,verts[faces[index].two].normY,verts[faces[index].two].normZ);
         glVertex3f(verts[faces[index].two].x,verts[faces[index].two].y,verts[faces[index].two].z);
         glNormal3f(verts[faces[index].three].normX,verts[faces[index].three].normY,verts[faces[index].three].normZ);
         glVertex3f(verts[faces[index].three].x,verts[faces[index].three].y,verts[faces[index].three].z);
    glEnd();
}

float p2wx(int xp){
	float newX = 0;
	newX = (2.0/((float)_windowHeight))  *  ((float)xp - ((float) _windowWidth/2.0));
	return newX;
}
float p2wy(int yp){
	float newY = 0;
	newY = (2.0/(float) _windowHeight) * (float) yp - 1.0;
	return newY;
}

void calculateUVW(MyVector lookAt, MyVector eyePoint)
{
   /*MyVector gazeVector = * new MyVector();
     MyVector upVector = (0,0,0,0,1,0);
     MyVector vectorW = * new MyVector();
     MyVector vectorV = * new MyVector();
     MyVector vectorU = * new MyVector();
     float mag = 0;
     
     //Step1
     gazeVector.endX = lookAt.endX - eyePoint.endX
     gazeVector.endY = lookAt.endY - eyePoint.endY
     gazeVector.endZ = lookAt.endZ - eyePoint.endZ
     
     //Step2. X
     mag = sqrt(pow(gazeVect.endX,2.0) + pow(gazeVect.endY,2.0) + pow(gazeVect.endZ,2.0));
     vectorW.endX = -(gazeVector.endX/mag);
     vectorW.endY = -(gazeVector.endY/mag);
     vectorW.endZ = -(gazeVector.endZ/mag);
     
     //Step3. Y
     float crossX;
     float crossY;
     float crossZ;
     crossX = upVector.endY * vectorW.endZ - upVector.endZ * vectorW.endY;
     crossY = upVector.endZ * vectorW.endX - upVector.endX * vectorW.endZ;
     crossZ = upVector.endX * vectorW.endY - upVector.endY * vectorW.endX;
     mag = sqrt(pow(crossX,2.0) + pow(crossY,2.0) + pow(crossZ,2.0));
     vectorU.endX = crossX/mag;
     vectorU.endY = crossY/mag;
     vectorU.endZ = crossZ/mag;
     
     //Step4. Z
     vectorV.endX = vectorW.endY * vectorU.endZ - vectorW.endZ * vectorU.endY;
     vectorV.endY = vectorW.endZ * vectorU.endX - vectorW.endX * vectorU.endZ;
     vectorV.endZ = vectorW.endX * vectorU.endY - vectorW.endY * vectorU.endX;
     
     
     */
}

void calculateEyePoint()
{
   /*EyePoint Formula
   x = radius*cos(phi)*cos(theta)
   y = radius*sin(phi)
   z = radius*cos(phi)*cos(90.0-theta)
   radius is used in zooming....
   Need to convert from degrees to radians!!!!!!!
   rad = deg * pi/180.0
   
   float phi, theta;
   phi = mouseY * (3.14159265/180.0);
   theta = mouseX * (3.14159265/180.0);
   
   
   eyePoint.endX = radius * cos(phi) * cos(theta);
   eyePoint.endY = radius * sin(phi);
   eyePoint.endZ = radius * cos(phi)* cos(90.0 - theta);
   */
}
void drawBunny(void){
   for(unsigned int j = 0; j < faces.size(); j++) {
       drawTria(j);
     }
}

GLuint createDL() {
	GLuint bunnyDL,loopDL;

	bunnyDL = glGenLists(1);
	loopDL = glGenLists(1);

	glNewList(bunnyDL,GL_COMPILE);
	drawBunny();
   //drawSnowMan();
	glEndList();

	glNewList(loopDL,GL_COMPILE);
	for(int i = -3; i < 3; i++)
		for(int j=-3; j < 3; j++) {
			glPushMatrix();
			glTranslatef(i*.5,0,j * .5);
			glCallList(bunnyDL);
			glPopMatrix();
		}
	glEndList();

	return(loopDL);
}

void renderBitmapString(float x, float y, float z, char *string)
{  
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }
}

void renderBitmapCharacher(float x, float y, float z, char *string)
{
  
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
  }
}

void drawWireFramePlane()
{
   
}

GLvoid DrawScene(void)
{
    /*1. GazeVect = lookAtVect - eyePointVect
      2. VectorW = -(GazeVect/Mag(GazeVect))
      3. VectorU = (UpVect X VectorW)/Mag(VectorW X UpVect)
      4. VectorV = VectorW X Vector U
      calculateEyePoint();
      calculateUVW(lookAt, eyePoint);
      */
    //cout << "A Redraw was Posted " << endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();
    glLoadIdentity();
    //gluLookAt(0, .5, .5, 0, 0, -1, 0, 1, 0); 
    gluLookAt(eyePoint.endX, eyePoint.endY, eyePoint.endZ,
              lookAt.endX, lookAt.endY, lookAt.endZ,
              up.endX, up.endY, up.endZ);
    //glPopMatrix();
    
    glPushMatrix();
    /////////////////////////////////////////////////////////////FPS CODE///////////////////////////////////////////
    frame++;
	FPStime=glutGet(GLUT_ELAPSED_TIME);
	
	if (FPStime - timebase > 1000) {
		sprintf(FPSDisplay,"FPS:%4.2f",
			frame*1000.0/(FPStime-timebase));
		timebase = FPStime;		
		frame = 0;
		//renderBitmapCharacher(-.2,.7,0,FPSDisplay);
	}
	renderBitmapString(-.2,.7,0,FPSDisplay);
	//////////////////////////////////////////////////////////////End FPS Code///////////////////////////////////////
    glPopMatrix();
    glPushMatrix();
    //glBegin(GL_LINE_LOOP);
    drawWireFramePlane();
    glBegin(GL_POLYGON);
    glColor3f(0,0,1.0);
    glVertex3f(-5.,0,5);
    glVertex3f(5,0,5);
    glVertex3f(5,0,-5);
    glVertex3f(-5,0,-5);
    glEnd();
   
    //glScalef(1.0/(float)max_extent, 1.0/(float)max_extent, 1.0/(float)max_extent);
    //translate the object to the orgin
    //glTranslatef(-(center.x), -(center.y), -(center.z));
     glCallList(DLid);
      glPopMatrix();
		// Update the title with the correct FPS
		char fps[50];
		//sprintf(fps, "Third Degree - %.2f FPS", GetFPS());
	//	SDL_WM_SetCaption(fps, 0);
	SDL_GL_SwapBuffers();
}

void setCameraMode(int width, int height) // reshape the window when it's moved or resized
{
   //GW = w;
	//GH = h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//if(ortho)
	//{
	   //glOrtho( -2.0*(float)width/height, 2.0*(float)width/height, -2.0, 2.0, 1.0, 15.0);
	//}
	//else
	//{
	   gluPerspective(90,(float)width/(float)height,1,15);
	//}
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, width, height);
	
	//DrawScene();
	//glutPostRedisplay();
}

GLboolean CheckKeys(void)
{
	if (_keys[SDLK_ESCAPE])
		return true;
   else if(_keys[SDLK_UP])
       forewardBackwardMovement+=.1;
   else if(_keys[SDLK_LEFT])
       leftRightMovement-=.1;
   else if(_keys[SDLK_DOWN])
       forewardBackwardMovement-=.1;
   else if(_keys[SDLK_RIGHT])
       leftRightMovement+=.1;
	return false;

	// Others are SDLK_LEFT, SDLK_UP, etc
}

Uint32 FPSTimer(Uint32 interval, void *param)
//int FPSTimer(int interval, int param)
{
   /*frame++;
	FPStime=glutGet(GLUT_ELAPSED_TIME);
	
	char s[11];
	if (FPStime - timebase > 1000) {
		sprintf(s,"FPS:%4.2f",
			frame*1000.0/(FPStime-timebase));
		timebase = FPStime;		
		frame = 0;
		cout << s << endl;
		renderBitmapCharacher(-.2,.7,0,s);
	}
	cout << "Complete" << endl;
	return 500;*/

}

int main(int argc, char *argv[]) {
    //Initialization
    glutInit(&argc, argv);
    _windowWidth = 600;
    _windowHeight = 600;
    mouseX = 0;
    mouseY = 0;
    max_x = max_y = max_z = FLT_MIN;
    min_x = min_y = min_z = FLT_MAX;
    cx =cy = cz = 0;
    max_extent = 1.0;
    eyePoint = * new MyVector(0,0,0,0,.5,.5);
    lookAt = * new MyVector(0,0,0,0,0,-1);
    up = *new MyVector(0,0,0,0,1,0);
    forewardBackwardMovement = 0;
    leftRightMovement = 0;
    //SDL INITIALIZATIONS
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "Unable to initialize SDL: " << SDL_GetError() << endl;
		exit(1);
	}

	if (SDL_SetVideoMode(_windowWidth, _windowHeight, 0, SDL_OPENGL) == NULL)
	{
		cout << "Unable to create OpenGL scene: " << SDL_GetError() << endl;
		exit(2);
	}
	glClearColor( 1.0, 1.0 ,1.0, 1);
	void *param;
	//TIMER CODE
	srand(50);
	SDL_Init(SDL_INIT_TIMER);
   SDL_AddTimer(Uint32 (1000), FPSTimer, param);
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
     } else {
       cout << "format is: meshparser filename" << endl;
     }

   //////////////////////////////End Mesh Code////////////
	// Set window title
	SDL_WM_SetCaption("Lab1", 0);
   
	int running = 1;
   DLid = createDL();
	while (running)
	{
		DrawScene();

		SDL_Event event;
      float mouseButtonDown = 0;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				running = 0;
         if(event.type == SDL_MOUSEMOTION)
         {
         //printf("mouse clicked at %d %d\n", x, GH-y-1);
             cout << "Mouse moving " << endl;
             printf("Moused moved to %lf, %lf\n", p2wx(event.motion.x), p2wy(_windowHeight - event.motion.y - 1));
                printf("Mouse moved by %d,%d to (%d,%d)\n", 
                       event.motion.xrel, event.motion.yrel,
                       event.motion.x, event.motion.y);
             if(mouseButtonDown)
             {
               //keep track of movement for camera motion, set mouse movement variable here.
               mouseX = p2wx(event.motion.xrel);
               mouseY = p2wy(event.motion.yrel);
             }
         }     
         if(event.type == SDL_MOUSEBUTTONDOWN)
         {
               printf("Mouse button %d pressed at (%d,%d)\n",
                      event.button.button, event.button.x, event.button.y);
               mouseButtonDown = 1;
         }
         if (event.type == SDL_MOUSEBUTTONUP)
         {
            printf("The mouse is up\n");
            mouseButtonDown = 0;
         }

			_keys = SDL_GetKeyState(NULL);
		}

		if (CheckKeys())
			running = 0;
	}

	SDL_Quit();
   return 0;
}
