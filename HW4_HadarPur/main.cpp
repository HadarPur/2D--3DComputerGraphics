#include <GLUT/GLUT.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

const int WIDTH = 600;
const int HEIGHT = 600;

const int MAX_POINTS = 100;
const double PI = 3.14;

int numOfPoints = 0;
double points_array[MAX_POINTS][2];
double eyeX = 1, eyeY = 1, eyeZ = 12;
double dx = 0, dy = 0, dz = 0;

double offset = 0;

// motion
double dir[3]; // x,y,z
double sight_angle = PI; // in radians
double pitch = 0;
double speed = 0, angular_speed = 0;

void init()
{
    glClearColor(0, 0, 0.4, 0); // set background color
}

// n is a number of sides
void DrawCylinder(int n, double topr, double bottomr)
{
    double alpha, teta = (2 * PI) / n;
    double delta = 0;
    for (alpha = 0; alpha <= 2 * PI + teta; alpha += teta)
    {
        glBegin(GL_POLYGON);
        glColor3d(1 - 0.7*fabs(sin(alpha)), 1 - fabs(sin(alpha)), (1 + cos(alpha)) / 2);
        glVertex3d(topr*sin(alpha), 1, topr*cos(alpha)); // 1
        glColor3d(1 - 0.7*fabs(sin(alpha + teta)), 1 - fabs(sin(alpha + teta)), (1 + cos(alpha + teta)) / 2);
        glVertex3d(topr*sin(alpha + teta), 1, topr*cos(alpha + teta)); // 2
        glVertex3d(bottomr*sin(alpha + teta + delta), 0, bottomr*cos(alpha + teta + delta)); // 3
        glColor3d(1 - 0.7*fabs(sin(alpha)), 1 - fabs(sin(alpha)), (1 + cos(alpha)) / 2);
        glVertex3d(bottomr*sin(alpha + delta), 0, bottomr*cos(alpha + delta)); // 4
        glEnd();
    }
    
}

void Draw2DShape()
{
    glColor3d(0.8, 0.8, 1);
    glBegin(GL_POLYGON);
    glVertex2d(-1, -1);
    glVertex2d(-1, 1);
    glVertex2d(1, 1);
    glVertex2d(1, -1);
    glEnd();
    
    
    glColor3d(0, 0, 0);
    glBegin(GL_LINES);
    glVertex2d(0, 1);
    glVertex2d(0, -1);
    glEnd();
    
    glPointSize(4);
    glColor3d(1, 0, 0);
    glBegin(GL_POINTS);
    for (int i=0; i<numOfPoints; i++)
    {
        glVertex2d(points_array[i][0], points_array[i][1]);
    }
    glEnd();
    
    glColor3d(1, 0, 0);
    glBegin(GL_POINTS);
    for (int i=0; i<numOfPoints; i++)
    {
        glVertex2d(-points_array[i][0], points_array[i][1]);
    }
    glEnd();
    
    glLineWidth(2);
    glColor3d(1, 1, 0);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i<numOfPoints; i++)
    {
        glVertex2d(points_array[i][0], points_array[i][1]);
    }
    glEnd();
    
    glColor3d(1, 1, 0);
    glBegin(GL_LINE_STRIP);
    for (int i=0; i<numOfPoints; i++)
    {
        glVertex2d(-points_array[i][0], points_array[i][1]);
    }
    glEnd();

}


void Draw3DShape()
{
    double hight = 0;
    for (int i=1; i<numOfPoints; i++)
    {
        glPushMatrix();
        hight += points_array[i-1][1] - points_array[i][1];
        glTranslated(0, -hight, 0);
        glScaled(1, points_array[i-1][1] - points_array[i][1], 1);
        DrawCylinder(40, points_array[i-1][0], points_array[i][0]);
        glPopMatrix();
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z-Buffer
 
    ////----------------------------------------------------------------------////
    ////--------------------------------- 3D ---------------------------------////
    ////----------------------------------------------------------------------////

    glViewport(0, 0, WIDTH/2, HEIGHT);
    glMatrixMode(GL_PROJECTION); // start defining the projection matrix
    glLoadIdentity();
    // left, right, bottom, top , near, far
    glFrustum(-1, 1, -1, 1, 0.8, 250);
    gluLookAt(eyeX, eyeY, eyeZ, // eye coordinates
              eyeX + dir[0], eyeY + dir[1], eyeZ + dir[2],  // Point of interest
              0, 1, 0);// UP
    // go back to "MODEL-VIEW" matrix
    glPushMatrix();
    glRotated(offset, 0, 1, 0);
    glTranslated(0, 2, 0);
    glScaled(6, 3, 6) ;
    Draw3DShape();
    glPopMatrix();
    
    
    ////----------------------------------------------------------------------////
    ////--------------------------------- 2D ---------------------------------////
    ////----------------------------------------------------------------------////
    glViewport(WIDTH/2, 0, WIDTH/2, HEIGHT);
    // prepare 2D
    glMatrixMode(GL_PROJECTION); // start defining the projection matrix
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glDisable(GL_DEPTH_TEST);
    // now go to model transformations
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Draw2DShape();
    
    glEnable(GL_DEPTH_TEST);
    
    glutSwapBuffers();
}


void idle()
{
    offset += 0.3;
    
    // compute ego-motion
    sight_angle += angular_speed;
    
    dir[0] = sin(sight_angle);
    dir[1] = sin(pitch);
    dir[2] = cos(sight_angle);
    
    eyeX += speed*dir[0];
    eyeY += speed*dir[1];
    eyeZ += speed*dir[2];
    
    glutPostRedisplay(); // call to refresh window
}

void special(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_UP: // forward
            speed += 0.01;
            break;
        case GLUT_KEY_DOWN:
            speed -= 0.01;
            break;
        case GLUT_KEY_LEFT:
            angular_speed += 0.003;
            break;
        case GLUT_KEY_RIGHT:
            angular_speed -= 0.003;
            break;
        case GLUT_KEY_PAGE_UP:
            pitch += 0.1;
            break;
        case GLUT_KEY_PAGE_DOWN:
            pitch -= 0.1;
            break;
    }
}

void mouse(int button, int state, int x, int y)
{
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && x > WIDTH/2 && numOfPoints<MAX_POINTS)
        {
            points_array [numOfPoints][0] = (x - 3*WIDTH/4) / 150.0;
            points_array [numOfPoints][1] = 2*(HEIGHT - y)/ 600.0 - 1;
            numOfPoints++;
        }
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    // add 2 frame buffers and Z-Buffer
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(200, 100);
    glutCreateWindow("2D -> 3D app");
    
    glutDisplayFunc(display);
    glutSpecialFunc(special); // for non-ascii keys
    glutMouseFunc(mouse);
    glutIdleFunc(idle);
    
    init();
    
    glutMainLoop();
    
    return 0;
}
