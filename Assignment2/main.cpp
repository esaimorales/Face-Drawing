//
//  main.cpp
//  Assignment2
//
//  Created by Esai Morales on 11/8/18.
//  Copyright © 2018 Esai Morales. All rights reserved.
//

#include <OpenGL/gl.h>
#include <OpenGl/glu.h>
#include <GLUT/glut.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <cmath>

#include <fstream>
#include <sstream>
#include <string>

#include <vector> 

// Supress deprecated GLU warnings
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using namespace std;

// Struct to store x, y, z coordinates of a vertex
struct vertex {
    float x, y, z;
};

struct v {
    float x, y, z;
};

struct triangle {
    int x, y, z;
};

enum {POINTS=1, WIRE, FILL};

int rendermode = FILL;

// The array to store the vertices from face-vertices.txt
vertex vertices[7061];

// The array to store the triangle indexes from face-index.txt
triangle triangles[13806];

// Used to store values of mouse coordinates on click
int OldX, OldY;

// Stores arbitrary values for the rotation
double AngleX, AngleY;

// Zoom value defines the xyz coordinate of the camera to zoom
float ZoomVal = 2;

void MenuValue(int option) {
    rendermode = option;
    glutPostRedisplay();
}

void Zoom(unsigned char key, int x, int y) {
    
    // 61 represents the "=" or "+" key
    if (key == 61) {
        ZoomVal-= .05;
    }
    
    // 45 represents the "-" (minus) button
    else if (key == 45) {
        ZoomVal+= .05;
    }
    
    glutPostRedisplay();
}

void MouseClick(int button, int state, int x, int y) {
    
    // If the left button is clicked the coordinates are saved for use in rotation
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        OldX = x;
        OldY = y;
    }
}

void init(void) {
    
    // Sets background color to black
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);
    
    // Initialize lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Set light position
    GLfloat light_pos[] = {0.5f, 0.5f, -5.3f, 1.f};//{ 2, 1, 0, 0 };
    
    // My addition
    /*
    GLfloat diffuse0[] = {1.0,0.0,0.0,1.0};
    GLfloat ambient0[] = {0.0,0.0,0.0,1.0};
    GLfloat specular0[] = {1.0,1.0,1.0,1.0};
    */
     
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    
    // My addition
    
    /*
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    */
     
    static GLfloat face_mat[] = {1.0f, 0.5f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, face_mat);
    
    // Initialize camera
    glMatrixMode(GL_PROJECTION);
    gluPerspective(50, 1, 0.1, 10);
    glMatrixMode(GL_MODELVIEW);
    
    // Initialize Menu and options
    glutCreateMenu(MenuValue);
    glutAddMenuEntry("Points", POINTS);
    glutAddMenuEntry("Wireframe", WIRE);
    glutAddMenuEntry("Solid", FILL);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // Zooming in and out
    glutKeyboardFunc(Zoom);
}


vector<float> computeNormal(vertex A, vertex B, vertex C){
    
    // Calculate the directional vectors
    
    vertex V, W;
    
    V.x = A.x - B.x;
    V.y = A.y - B.y;
    V.z = A.z - B.z;

    W.x = B.x - C.x;
    W.y = B.y - C.y;
    W.z = B.z - C.z;
    
    // Calculates the normal vector relative to triangle plane
    
    vector<float> normal;

    float n1 = (V.y * W.z) - (V.z * W.y);
    float n2 = (V.z * W.x) - (V.x * W.z);
    float n3 = (V.x * W.y) - (V.y * W.x);
    
    float len  = abs(n1) + abs(n2) + abs(n3);
    
    normal.push_back(n3/len);
    normal.push_back(n2/len);
    normal.push_back(n1/len);
    
    return normal;
}


void drawTriangle(int first, int second, int third) {
    
    if(rendermode == 1){
        glBegin(GL_POINTS);
    }
    else{
        glBegin(GL_TRIANGLES);
    }
    
    vector<float> normal = computeNormal(vertices[first], vertices[second], vertices[third]);
    
    glNormal3f(normal[0], normal[1], normal[2]);
    glVertex3f(vertices[first].x, vertices[first].y, vertices[first].z);
    glVertex3f(vertices[second].x, vertices[second].y, vertices[second].z);
    glVertex3f(vertices[third].x, vertices[third].y, vertices[third].z);
    
    glEnd();
};


void initObject() {
    
    cout << "reading files..." << endl;
    
    // Read from file and create vertex objects
    // Add all vertices to vertices array
    
    ifstream file("/Users/esaimorales/Desktop/Face-Drawing/Assignment2/face-vertices.txt");
    
    if (!file)
    {
        cout << "error opening input file, please specify correct path" << endl;
    }
    
    
    string str;
    int i = 0;
    
    while (getline(file, str)){
        
        string coordinate;
        stringstream stream(str);
        float coordinates[3];
        
        int j = 0;
        while(getline(stream, coordinate, ','))
        {
            coordinates[j] = stof(coordinate);
            j++;
        }
        
        vertex temp;
        temp.x = coordinates[0];
        temp.y = coordinates[1];
        temp.z = coordinates[2];
        
        vertices[i] = temp;
        
        i++;
    }
    
    // Read from file and create triangle objects
    // Add all triangles to triangles array
    
    ifstream file2("/Users/esaimorales/Desktop/Face-Drawing/Assignment2/face-index.txt");
    
    if (!file2)
    {
        cout << "error opening input file, please specify correct path" << endl;
    }
    
    i = 0;
    
    while(getline(file2, str)){
        
        string index;
        stringstream stream(str);
        int indexes[3];
        
        int j = 0;
        while(getline(stream, index, ','))
        {
            indexes[j] = stoi(index);
            j++;
        }
        
        triangle temp;
        temp.x = indexes[0];
        temp.y = indexes[1];
        temp.z = indexes[2];
        triangles[i] = temp;
        
        i++;
    }
    
}


void redraw(void) {
    
    // Print OpenGL errors
    GLenum err_code;
    do {
        err_code = glGetError();
        if (err_code != GL_NO_ERROR)
        printf("Error: %s\n", gluErrorString(err_code));
    } while (err_code != GL_NO_ERROR);
    
    // Clear buffer data
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // Set camera
    glLoadIdentity();
    gluLookAt(ZoomVal, ZoomVal, ZoomVal, 0, 0, 0, 0, 1, 0);
    glRotated(AngleX, 0, 1, 0);
    glRotated(AngleY, 1, 0, 0);
    
    
    // Set color
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    switch(rendermode) {
            
        case FILL:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glShadeModel(GL_FLAT);
            break;
            
        case POINTS:
            glPointSize(2);
            break;
            
        case WIRE:
            glColor4f(1.0f, 0.5f, 0.0f, 1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
            
        default:
            break;
    }
    
    // Display face
    
    for(int i = 0; i < 13806; i++){
        drawTriangle(triangles[i].x, triangles[i].y, triangles[i].z);
    }
    
    
    // Flush data
    glFlush();
}

void RotateObject(int x, int y) {
    
    double diffX, diffY;
    
    // Calculate the difference of old x value to current
    diffX = x - OldX;
    diffY = y - OldY;
    
    // Arbitrarily scales the difference
    diffX = diffX / 4;
    diffY = diffY / 4;
    
    // Creates and alters the angle to rotate by
    AngleX += diffX;
    AngleY += diffY;
    
    // Reassigns x and y value to be continuously used over and over
    OldX = x;
    OldY = y;
    
    glutPostRedisplay();
}

int main(int argc, char * argv[]) {
    
    initObject();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH);
    
    /* TODO: 1) add lighting/shading 2) add POINTS option */
    
    glutInitWindowSize(700, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("object representation");
    init();
    glutDisplayFunc(redraw);
    
    // Rotational
    glutMotionFunc(RotateObject);
    glutMouseFunc(MouseClick);
    
    glutMainLoop();
    return 0;
}

