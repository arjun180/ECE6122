// Draw an Icosahedron
// ECE4893/8893 Project 4
// YOUR NAME HERE

#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#define NFACE 20
#define NVERTEX 12

#define X .525731112119133606 
#define Z .850650808352039932

static const float DEG2RAD = M_PI/180;
static int updateRate = 10;

float* R;
float* G;
float* B;
int faceCount = 0;




// These are the 12 vertices for the icosahedron
static GLfloat vdata[NVERTEX][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
static GLint tindices[NFACE][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

int testNumber; // Global variable indicating which test number is desired
int depthNumber;

GLfloat rotX = 0.0;
GLfloat rotY = 0.0;

void init()
{
  //select clearing (background) color
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
}

void drawtriangle(float *v1, float *v2, float *v3) {

  
   glBegin(GL_TRIANGLES);   
      glColor3f(R[faceCount],G[faceCount],B[faceCount]); 
      faceCount++;
      glNormal3fv(v1); glVertex3fv(v1);    
      glNormal3fv(v2); glVertex3fv(v2);    
      glNormal3fv(v3); glVertex3fv(v3);    
   glEnd(); 


   glBegin(GL_LINE_LOOP); 

      glColor3f(1.0, 1.0, 1.0); 
      glNormal3fv(v1); glVertex3fv(v1);    
      glNormal3fv(v2); glVertex3fv(v2);    
      glNormal3fv(v3); glVertex3fv(v3);    
   glEnd(); 

}


void normalize(float v[3]) {    
   GLfloat d = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]); 
   
   v[0] /= d; v[1] /= d; v[2] /= d; 
}

void subdivide(float *v1, float *v2, float *v3) 
{ 
   GLfloat v12[3], v23[3], v31[3];    
   GLint i;

   for (i = 0; i < 3; i++) { 
      v12[i] = v1[i]+v2[i]; 
      v23[i] = v2[i]+v3[i];     
      v31[i] = v3[i]+v1[i];    
   } 
   normalize(v12);    
   normalize(v23); 
   normalize(v31); 
   drawtriangle(v1, v12, v31);    
   drawtriangle(v2, v23, v12);    
   drawtriangle(v3, v31, v23);    
   drawtriangle(v12, v23, v31); 
}


void subdivide1(float *v1, float *v2, float *v3, long depth)
{
   GLfloat v12[3], v23[3], v31[3];
   GLint i;

   if (depth == 0) {
      drawtriangle(v1, v2, v3);
      return;
   }
   for (i = 0; i < 3; i++) {
      v12[i] = v1[i]+v2[i];
      v23[i] = v2[i]+v3[i];
      v31[i] = v3[i]+v1[i];
   }
   normalize(v12);
   normalize(v23);
   normalize(v31);
   subdivide1(v1, v12, v31, depth-1);
   subdivide1(v2, v23, v12, depth-1);
   subdivide1(v3, v31, v23, depth-1);
   subdivide1(v12, v23, v31, depth-1);
}



void reshape(int w, int h)
{
  glViewport(0,0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)-w, (GLdouble)w);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void timer(int)
{
  glutPostRedisplay();
  glutTimerFunc(1000.0 / updateRate, timer, 0);
} 


void normcrossprod(float v1[3], float v2[3], float out[3]) 
{ 
   // GLint i, j; 
   // GLfloat length;

   out[0] = v1[1]*v2[2] - v1[2]*v2[1]; 
   out[1] = v1[2]*v2[0] - v1[0]*v2[2]; 
   out[2] = v1[0]*v2[1] - v1[1]*v2[0]; 
   normalize(out); 
}



// Test cases.  Fill in your code for each test case

void Test1()
{
  faceCount = 0;
  GLfloat radius = 1.0;
 
  for (int i = 0; i < 20; i++) {    

  drawtriangle(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0], &vdata[tindices[i][2]][0]);
     /* color information here */ 
  //   glBegin(GL_TRIANGLES);    
  //   glColor3f(R[i],G[i],B[i]);    
  //   glVertex3fv(&vdata[tindices[i][0]][0]); 
  //   glVertex3fv(&vdata[tindices[i][1]][0]); 
  //   glVertex3fv(&vdata[tindices[i][2]][0]); 
    
  
  // glEnd();
  // glBegin(GL_LINE_LOOP);    
  // glColor3f(1.0, 1.0, 1.0); 
  
     
  //    /* color information here */ 
        
  //   glVertex3fv(&vdata[tindices[i][0]][0]); 
  //   glVertex3fv(&vdata[tindices[i][1]][0]); 
  //   glVertex3fv(&vdata[tindices[i][2]][0]); 
    
  
  // glEnd();
}


}

void Test2(){

// Try rotating

glRotatef(rotX, 1.0, 0.0, 0.0);
glRotatef(rotY, 0.0, 1.0, 0.0);
rotX += 1.0;
rotY -= 1.0;
Test1();

}


void Test3(){
faceCount = 0;
for (int i = 0; i < 20; i++) { 
 subdivide(&vdata[tindices[i][0]][0],       
           &vdata[tindices[i][1]][0],       
           &vdata[tindices[i][2]][0]); 
}
}



void Test4(){

glRotatef(rotX, 1.0, 0.0, 0.0);
glRotatef(rotY, 0.0, 1.0, 0.0);
rotX += 1.0;
rotY -= 1.0;
Test3();

}

void Test5(int depth){
  faceCount = 0;
for (int i = 0; i < 20; i++) { 

subdivide1(&vdata[tindices[i][0]][0],       
         &vdata[tindices[i][1]][0],       
         &vdata[tindices[i][2]][0],
         depth); 
}



}

void Test6(int depth){

glRotatef(rotX, 1.0, 0.0, 0.0);
glRotatef(rotY, 0.0, 1.0, 0.0);
rotX += 1.0;
rotY -= 1.0;
Test5(depth);


}




void display(void)
{
  static int pass;
  
  // cout << "Displaying pass " << ++pass << endl;
  // clear all
  glClear(GL_COLOR_BUFFER_BIT);
  // Clear the matrix
  glLoadIdentity();
  // Set the viewing transformation
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glTranslatef(250, 250, 0);
  glScalef(100.0, 100.0, 0);
  switch(testNumber){
    case 1: Test1();
            break;
    case 2: Test2();
            break;
    case 3: Test3();
            break;

    case 4: Test4();
            break;

    case 5: Test5(depthNumber);
            break;
    
    case 6: Test6(depthNumber);
            break;

  }
  // Flush buffer
  //glFlush(); // If single buffering
  glutSwapBuffers(); // If double buffering
}



int main(int argc, char** argv)
{
  if (argc < 2)
    {
      std::cout << "Usage: icosahedron testnumber" << endl;
      exit(1);
    }
  // Set the global test number
  testNumber = atol(argv[1]);

  int d;

  if (testNumber == 1 || testNumber ==2){
    d = 0;
  }

  if (testNumber == 3 || testNumber ==4){
     d = 1;
  }

  if (testNumber == 5|| testNumber ==6){
    d = atol (argv[2]);
  }




  
  if (testNumber > 4){

    if (argc < 3)
    {
      std::cout << "More number of arguments required" << endl;
      exit(1);
    }

    depthNumber = atol(argv[2]);
  }

  
  // Initialize glut  and create your window here
  glutInit(&argc, argv);

  int size = (int) 20*pow(4,d);

  R = new float[size];
  G = new float[size];
  B = new float[size];

  srand(time(NULL));

  for (int t = 0; t < size; ++t){ 
    R[t] = ((float) rand() / (RAND_MAX));
    G[t] = ((float) rand() / (RAND_MAX));
    B[t] = ((float) rand() / (RAND_MAX));
  } 


  
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Icosahedron");
  init();

  // Set your glut callbacks here
  glutDisplayFunc(display);
  // Enter the glut main loop here
  glutReshapeFunc(reshape);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  glutMainLoop();
  return 0;

}

