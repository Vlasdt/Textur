#define _USE_MATH_DEFINES  
#include <cmath>


#define GL_BGR 0x80E0
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <windows.h>

#include <fstream>


using namespace std;

GLuint texture;
const int SEGMENTS = 64;

bool LoadBMP(const char* filename) {
    if (!filename) {
        cerr << "Error: Null filename pointer" << endl;
        return false;
    }

    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Could not open file " << filename << endl;
        return false;
    }
    unsigned char header[54];
    file.read(reinterpret_cast<char*>(header), 54);
    if (file.gcount() != 54) {
        cerr << "Error: Not a valid BMP file" << endl;
        return false;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        cerr << "Error: Not a valid BMP file" << endl;
        return false;
    }

    int dataPos = *(int*)&header[0x0A];
    int imageSize = *(int*)&header[0x22];
    int width = *(int*)&header[0x12];
    int height = *(int*)&header[0x16];

    if (imageSize == 0) imageSize = width * height * 3;
    if (dataPos == 0) dataPos = 54;

    unsigned char* data = new unsigned char[imageSize];
    if (!data) {
        cerr << "Error: Memory allocation failed" << endl;
        return false;
    }

    file.seekg(dataPos, ios::beg);
    file.read(reinterpret_cast<char*>(data), imageSize);
    file.close();

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    delete[] data;
    return true;
}

void InitOpenGL(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)width / height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glEnable(GL_TEXTURE_2D);
}

void DrawCylinder(float radius, float height) {
    float angleStep = 2.0f * M_PI / SEGMENTS;
    LoadBMP("colaside.bmp");
    // сторона
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        float tx = (float)i / SEGMENTS;

        glTexCoord2f(tx, 0.0f);
        glVertex3f(x, -height / 2, z);
        glTexCoord2f(tx, 1.0f); 
        glVertex3f(x, height / 2, z);
       
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        float tx = (float)i / SEGMENTS;

        glVertex3f(x, height / 2, z);
        glVertex3f(x * 0.9, height / 2+0.2, z*0.9);

    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        float tx = (float)i / SEGMENTS;

        glVertex3f(x, -height / 2, z);
        glVertex3f(x * 0.9, -height / 2 - 0.2, z * 0.9);

    }
    glEnd();

    //top
    LoadBMP("colatop.bmp");
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f); glVertex3f(0, height / 2+0.2, 0);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius*0.9 * cos(angle);
        float z = radius*0.9 * sin(angle);
        float tx = 0.5f + 0.5f * cos(angle)*0.8;
        float ty = 0.5f + 0.5f * sin(angle)*0.8;

        glTexCoord2f(tx, ty); glVertex3f(x , height / 2 + 0.2, z);
    }
    glEnd();

    // Нижняя крышка
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, -height / 2-0.2, 0);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius*0.9 * cos(angle);
        float z = radius*0.9 * sin(angle);
        float tx = 0.5f + 0.5f * cos(angle)*0.8;
        float ty = 0.5f + 0.5f * sin(angle)*0.8;

        glTexCoord2f(tx, ty); glVertex3f(x, -height / 2-0.2, z);
    }
    glEnd();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(3, 3, 3, 0, 0, 0, 0, 1, 0);

    static float angle = 0;
    angle += 0.5f;
    if (angle > 360) angle -= 360;

    glRotatef(angle, 0, 0, 1);
    DrawCylinder(1.0f, 3.0f);

    glutSwapBuffers();
}

void idle() {
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("cola");



    InitOpenGL(1200, 900);

    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}