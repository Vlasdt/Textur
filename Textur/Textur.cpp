#define _USE_MATH_DEFINES  
#include <cmath>
#define GL_BGR 0x80E0
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <windows.h>
#include <fstream>

using namespace std;

GLuint texture_side, texture_top;
const int SEGMENTS = 64;

// Параметры освещения
GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 1.0f };
GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
//GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Параметры материала
GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
//GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat mat_shininess[] = { 50.0f };

bool LoadBMP(const char* filename, GLuint& texture) {

    ifstream file(filename, ios::binary);
    unsigned char header[54];
    file.read(reinterpret_cast<char*>(header), 54);


    int dataPos = *(int*)&header[0x0A];
    int imageSize = *(int*)&header[0x22];
    int width = *(int*)&header[0x12];
    int height = *(int*)&header[0x16];

    if (imageSize == 0) imageSize = width * height * 3;
    if (dataPos == 0) dataPos = 54;

    unsigned char* data = new unsigned char[imageSize];


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

    // Настройка освещения
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Настройка материала
    //glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glEnable(GL_TEXTURE_2D);
}

void DrawCylinder(float radius, float height) {
    float angleStep = 2.0f * M_PI / SEGMENTS;

    // Боковая поверхность
    glBindTexture(GL_TEXTURE_2D, texture_side);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        float tx = (float)i / SEGMENTS;

        glNormal3f(cos(angle), 0.0f, sin(angle));

        glTexCoord2f(tx, 0.0f);
        glVertex3f(x, -height / 2, z);
        glTexCoord2f(tx, 1.0f);
        glVertex3f(x, height / 2, z);
    }
    glEnd();

    // Верхний обод
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        glNormal3f(cos(angle) * 0.7f, 0.7f, sin(angle) * 0.7f);

        glVertex3f(x, height / 2, z);
        glVertex3f(x * 0.9, height / 2 + 0.2, z * 0.9);
    }
    glEnd();

    // Нижний обод
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        glNormal3f(cos(angle) * 0.7f, -0.7f, sin(angle) * 0.7f);

        glVertex3f(x, -height / 2, z);
        glVertex3f(x * 0.9, -height / 2 - 0.2, z * 0.9);
    }
    glEnd();

    // Верхняя крышка
    glBindTexture(GL_TEXTURE_2D, texture_top);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f); 
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, height / 2 + 0.2, 0);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius * 0.9 * cos(angle);
        float z = radius * 0.9 * sin(angle);
        float tx = 0.5f + 0.5f * cos(angle) * 0.8;
        float ty = 0.5f + 0.5f * sin(angle) * 0.8;

        glTexCoord2f(tx, ty);
        glVertex3f(x, height / 2 + 0.2, z);
    }
    glEnd();

    // Нижняя крышка
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f); 
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0, -height / 2 - 0.2, 0);
    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = i * angleStep;
        float x = radius * 0.9 * cos(angle);
        float z = radius * 0.9 * sin(angle);
        float tx = 0.5f + 0.5f * cos(angle) * 0.8;
        float ty = 0.5f + 0.5f * sin(angle) * 0.8;

        glTexCoord2f(tx, ty);
        glVertex3f(x, -height / 2 - 0.2, z);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(3, 3, 3, 0, 0, 0, 0, 1, 0);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    static float angle = 0;
    angle += 0.5f;
    if (angle > 360) angle -= 360;

    glRotatef(angle, 0, 1, 0);
    DrawCylinder(1.0f, 3.0f);

    glutSwapBuffers();
}

void idle() {
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 900);
    glutCreateWindow("Cola");

    
    LoadBMP("colaside.bmp", texture_side);
    LoadBMP("colatop.bmp", texture_top);


    InitOpenGL(1200, 900);

    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}