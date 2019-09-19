//
//  main.cpp
//  HW1_SamiaKabir
//
//  Created by Samia Kabir on 9/6/19.
//  Copyright © 2019 Samia Kabir. All rights reserved.
//
//


#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>
#include<unistd.h>
#include <vector>
#include "Eigen/Dense"
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace Eigen;

double ballx, bally, ballz;  //The position of the ball - you can set this in your code
double boxxl, boxxh, boxyl, boxyh, boxzl, boxzh;  // The low and high x, y, z values for the box sides

Vector3d X;   // position vector
Vector3d V;   // velocity vector
Vector3d a;           // acceleration
Vector3d W;        //wind velocity
double timestep;    //time step
double timestep_remain;
double tmax;
double mass;         //ball mass
double d;            //air resistance
double f;            //collision time step
Vector3d g(0,-10,0);        // gravitational pull
int rotateon;
int radius=5;

double E;      //elasticity component
double u;      //frictional component

int target_FPS=30;
double time_per_frame=(double)(1000/target_FPS);
int g_start_time;
int g_current_frame_number;

double xmin, xmax, ymin, ymax, zmin, zmax;
double maxdiff;
Vector3d F, F_g, F_air, F_wind;
Vector3d Vnew, Xnew, Vcollide;
Vector3d plane_normal, V_nc, V_tc, V_n, V_t;

int lastx, lasty;
int xchange, ychange;
float spin = 0.0;
float spinup = 0.0;

void simulation_loop(int l);

void display(void)
{
    GLfloat box_ambient[] = { 0.1, 0.1, 0.1 };
    GLfloat smallr00[] = { 0.1, 0.0, 0.0 };
    GLfloat small0g0[] = { 0.0, 0.1, 0.0 };
    GLfloat small00b[] = { 0.0, 0.0, 0.1 };
    GLfloat smallrg0[] = { 0.1, 0.1, 0.0 };
    GLfloat smallr0b[] = { 0.1, 0.0, 0.1 };
    GLfloat small0gb[] = { 0.0, 0.1, 0.1 };
    GLfloat smallrgb[] = { 0.1, 0.1, 0.1 };
    
    GLfloat box_diffuse[] = { 0.7, 0.7, 0.7 };
    GLfloat box_specular[] = { 0.1, 0.1, 0.1 };
    GLfloat box_shininess[] = { 10.0 };
    GLfloat ball_ambient[] = { 0.4, 0.0, 0.0 };
    GLfloat ball_diffuse[] = { 0.3, 0.0, 0.0 };
    GLfloat ball_specular[] = { 0.3, 0.3, 0.3 };
    GLfloat ball_shininess[] = { 30.0 };
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    
    //rotate the view
    glRotatef(spinup, 1.0, 0.0, 0.0);
    glRotatef(spin, 0.0, 1.0, 0.0);
    
    /*
     8 vertices:
     glVertex3f(boxxl, boxyl, boxzl);
     glVertex3f(boxxh, boxyl, boxzl);
     glVertex3f(boxxh, boxyh, boxzl);
     glVertex3f(boxxl, boxyh, boxzl);
     glVertex3f(boxxl, boxyl, boxzh);
     glVertex3f(boxxh, boxyl, boxzh);
     glVertex3f(boxxh, boxyh, boxzh);
     glVertex3f(boxxl, boxyh, boxzh);
     */
    
    //Draw the box
    //set material parameters
    glMaterialfv(GL_FRONT, GL_AMBIENT, box_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, box_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, box_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, box_shininess);
    
    glBegin(GL_QUADS);
    //back face
    glMaterialfv(GL_FRONT, GL_AMBIENT, smallrgb);
    glVertex3f(boxxl, boxyl, boxzl);
    glVertex3f(boxxh, boxyl, boxzl);
    glVertex3f(boxxh, boxyh, boxzl);
    glVertex3f(boxxl, boxyh, boxzl);
    
    //left face
    glMaterialfv(GL_FRONT, GL_AMBIENT, small0g0);
    glVertex3f(boxxl, boxyl, boxzh);
    glVertex3f(boxxl, boxyl, boxzl);
    glVertex3f(boxxl, boxyh, boxzl);
    glVertex3f(boxxl, boxyh, boxzh);
    
    //right face
    glMaterialfv(GL_FRONT, GL_AMBIENT, small00b);
    glVertex3f(boxxh, boxyl, boxzh);
    glVertex3f(boxxh, boxyh, boxzh);
    glVertex3f(boxxh, boxyh, boxzl);
    glVertex3f(boxxh, boxyl, boxzl);
    
    //bottom face
    glMaterialfv(GL_FRONT, GL_AMBIENT, smallrg0);
    glVertex3f(boxxh, boxyl, boxzh);
    glVertex3f(boxxh, boxyl, boxzl);
    glVertex3f(boxxl, boxyl, boxzl);
    glVertex3f(boxxl, boxyl, boxzh);
    
    //top face
    glMaterialfv(GL_FRONT, GL_AMBIENT, smallr0b);
    glVertex3f(boxxh, boxyh, boxzh);
    glVertex3f(boxxl, boxyh, boxzh);
    glVertex3f(boxxl, boxyh, boxzl);
    glVertex3f(boxxh, boxyh, boxzl);
    
    //front face
    glMaterialfv(GL_FRONT, GL_AMBIENT, small0gb);
    glVertex3f(boxxh, boxyl, boxzh);
    glVertex3f(boxxl, boxyl, boxzh);
    glVertex3f(boxxl, boxyh, boxzh);
    glVertex3f(boxxh, boxyh, boxzh);
    
    glEnd();
    
    //draw the ball
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ball_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ball_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ball_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, ball_shininess);
    glPushMatrix();
    glTranslatef(ballx, bally, ballz);
    glutSolidSphere(5, 10, 10);
    glPopMatrix();
    
    glPopMatrix();
    glutSwapBuffers();

   // cout<< ballx<< " "<<bally<<" "<<ballz<<endl;
   // simulation_loop();
}

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    // Enable Z-buffering, backface culling, and lighting
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Set eye point and lookat point
    gluLookAt(0, 225, 300, 0, 0, 0, 0, 1, 0);
    
    // Set up lights
    GLfloat light0color[] = { 5.0, 2.0, 2.0 };
    GLfloat light0pos[] = { 400, 500, 300 };
    GLfloat light1color[] = { 1.0, 2.0, 5.0 };
    GLfloat light1pos[] = { 0, 300, 300 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0color);
    glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1color);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1color);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1color);
    g_start_time = glutGet(GLUT_ELAPSED_TIME);
    g_current_frame_number = 0;
    
    //Initialize ball position
    ballx = 0.0; bally = 50.0; ballz = 0.0;
    
    //initializations of vectors
    X[0]= ballx; X[1]=bally; X[2]=ballz;     // position vector
    a[0]=0; a[1]=-10; a[2]=0;
    V[0]=-30; V[1]=30; V[2]=0;                 // velocity vector
    W[0]=20; W[1]=5; W[2]=-5;
    //W[0]=0; W[1]=0; W[2]=0;                   //wind velocity
    timestep=0.005;
    tmax=0.50;
    d=0.05;
    mass= 1;
    E=0.95;
    u=0.2;
    
    
    //Initialize box boundaries
    boxxl = -100;
    boxxh = 100;
    boxyl = -100;
    boxyh = 100;
    boxzl = -100;
    boxzh = 100;
}

void reshapeFunc(GLint newWidth, GLint newHeight)
{
    if (newWidth > newHeight) // Keep a square viewport
        glViewport((newWidth - newHeight) / 2, 0, newHeight, newHeight);
    else
        glViewport(0, (newHeight - newWidth) / 2, newWidth, newWidth);
    init();
    glutPostRedisplay();
}

void rotateview(void)
{
    if (rotateon) {
        spin += xchange / 250.0;
        if (spin >= 360.0) spin -= 360.0;
        if (spin < 0.0) spin += 360.0;
        spinup -= ychange / 250.0;
        if (spinup > 89.0) spinup = 89.0;
        if (spinup < -89.0) spinup = -89.0;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                lastx = x;
                lasty = y;
                xchange = 0;
                ychange = 0;
                rotateon = 1;
            }
            else if (state == GLUT_UP) {
                xchange = 0;
                ychange = 0;
                rotateon = 0;
            }
            break;
            
        default:
            break;
    }
}

void motion(int x, int y)
{
    xchange = x - lastx;
    ychange = y - lasty;
}


void simulation_loop(int l)
{
    
    auto start = std::chrono::system_clock::now();

    
    for(double t=timestep;t<=tmax;t+=timestep){
    timestep_remain=timestep;
    
    while(timestep_remain>0)
    {
        ///////////////////////////// find acceleration////////////////////
        F_air= -d*(V);
        F_g= mass*g;
        F_wind= d*(W);
        F= F_air+F_g+F_wind;
        a= F/mass;
        
        
        ///////////////////////////// find Xnew, Vnew////////////////////
        
        Vnew= V+timestep_remain*a;
        Xnew= X+ timestep_remain*V;
        
        ////////////////////////////Collision detection//////////////////
        if((Xnew[0]<=-100+radius || Xnew[0]>= 100-radius) ||(Xnew[1]<=-100+radius || Xnew[1]>= 100-radius) ||(Xnew[2]<=-100+radius || Xnew[2]>= 100-radius)){
            
            if(Xnew[0]<=-100+radius || Xnew[0]>= 100-radius){
                
                if(Xnew[0]<=-100+radius)
                {
                    f= ((-100+radius) - X[0])/(Xnew[0]-X[0]);
                    plane_normal[0]=1; plane_normal[1]=0; plane_normal[2]=0;
                }
                
                else
                {
                    f= ((100-radius) - X[0])/(Xnew[0]-X[0]);
                    plane_normal[0]=-1; plane_normal[1]=0; plane_normal[2]=0;
                }
                
                
            }
            
            
            else if(Xnew[1]<=-100+radius || Xnew[1]>= 100-radius){
                
                if(Xnew[1]<=-100+radius)
                {
                    f= ((-100+radius) - X[1])/(Xnew[1]-X[1]);
                    plane_normal[0]=0; plane_normal[1]=1; plane_normal[2]=0;
                }
                
                else
                {
                    f= ((100-radius) - X[1])/(Xnew[1]-X[1]);
                    plane_normal[0]=0; plane_normal[1]=-1; plane_normal[2]=0;
                }
                
                
            }
            
            else{
                if(Xnew[2]<=-100+radius)
                {
                    f= ((-100+radius) - X[2])/(Xnew[2]-X[2]);
                    plane_normal[0]=0; plane_normal[1]=0; plane_normal[2]=1;
                }
                
                else
                {
                    f= ((100-radius) - X[2])/(Xnew[2]-X[2]);
                    plane_normal[0]=0; plane_normal[1]=0; plane_normal[2]=-1;
                }
            }
            ////////////////////////////Collision determination//////////////////
            
            Vcollide= V+ 0.9*f*timestep_remain*a;
            Xnew= X+0.9*f*timestep_remain*V;
            
            ////////////////////////////Collision response//////////////////
            
            V_n= (Vcollide.dot(plane_normal))* (plane_normal);
            V_t= Vcollide-V_n;
            
            V_nc= -E *V_n;
            V_tc=(1-u)* V_t;
            
            Vnew= V_nc+V_tc;
            
            timestep_remain=(1-f)*timestep_remain;
            
        }
        
        else
        {
            timestep_remain=0;
        }
        
        
        ////////////////////////////set new state///////////////////////
        X=Xnew;
        V=Vnew;
        

    }
    
 }

      auto end = std::chrono::system_clock::now();
    
      std::chrono::duration<double> elapsed_seconds = end-start;
    

    
     if(elapsed_seconds.count()<0.3333){
        sleep((0.3333- elapsed_seconds.count()));
       //printf("%f \n", 0.3333- elapsed_seconds.count());

     }
    
    ballx=X[0]; bally=X[1]; ballz=X[2];
    display();
//    printf("%f %f %f \n",X[1]-(-95),V.norm(),a[1]);
    if((X[1]-(-95)<=3) && (V.norm()<=7) && a[1]<0)
    {
        X[1]=-100;
        V[0]=0; V[1]=0; V[2]=0;
        a[0]=0; a[1]=0; a[2]=0;
    }
    else
       glutTimerFunc(0, simulation_loop,0);
}

int main(int argc, char** argv)
{
    GLint SubMenu1, SubMenu2, SubMenu3, SubMenu4;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Bouncing Ball Demo");
    init();
    rotateon = 0;

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(rotateview);
    glutReshapeFunc(reshapeFunc);
    
    simulation_loop(0);
    glutMainLoop();
    return 0;
}
