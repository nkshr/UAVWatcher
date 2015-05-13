#define WIDTH 640
#define HEIGHT 480
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "wsock32.lib")

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>
#include <WinSock2.h>
#include <iostream>
#include <GL/glut.h>

using namespace cv;

class Camera
{
public:
	Mat rmat, tmat;
	Mat eye, up, center;
	Camera(double x, double y, double z);
	void roll(double angle);
	void pitch(double angle);
	void yaw(double angle);
	void translate(double x, double y, double z);
	void update();
	void resetRmat();
};

void display();
void init();
void timer(int value);
void postProcess();
void line3D(GLfloat x1, GLfloat y1,
	GLfloat z1, GLfloat x2,
	GLfloat y2, GLfloat z2);
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);
void rotate_uav();