#include "uav_watcher.h"
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_lib.hpp>

using namespace cv;
using namespace std;

GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat lightpos[] = { 200.0, 150.0, 500.0, 1.0 };

void line3D(GLfloat x1, GLfloat y1,
	GLfloat z1, GLfloat x2,
	GLfloat y2, GLfloat z2)
{
	glLineWidth(3.0);
	glBegin(GL_LINES);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glEnd();
}

Camera::Camera(double x, double y, double z)
{
	rmat = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
	eye = (Mat_<double>(3, 1) << 0, 0, 0);
	up = (Mat_<double>(3, 1) << 0, 1, 0);
	center = (Mat_<double>(3, 1) << 0, 0, -1);
	translate(x, y, z);
}

void Camera::roll(double angle)
{
	Mat cmat = (Mat_<double>(3, 3) <<
		0, -up.at<double>(2, 0), up.at<double>(1, 0),
		up.at<double>(2, 0), 0, -up.at<double>(0, 0),
		-up.at<double>(1, 0), up.at<double>(0, 0), 0);
	angle = (CV_PI / 180) * angle;
	Mat rollingMat = Mat::eye(3, 3, CV_64F) + sin(angle) * cmat + (1 - cos(angle)) * cmat * cmat;
	rmat = rollingMat * rmat;
}

void Camera::pitch(double angle)
{
	Mat temp = center - eye;
	Mat rAxis = temp.cross(up);
	Mat cmat = (Mat_<double>(3, 3) <<
		0, -rAxis.at<double>(2, 0), rAxis.at<double>(1, 0),
		rAxis.at<double>(2, 0), 0, -rAxis.at<double>(0, 0),
		-rAxis.at<double>(1, 0), rAxis.at<double>(0, 0), 0);
	angle = (CV_PI / 180) * angle;
	Mat pitchingMat = Mat::eye(3, 3, CV_64F) + sin(angle) * cmat + (1 - cos(angle)) * cmat * cmat;
	rmat = pitchingMat * rmat;
}

void Camera::yaw(double angle)
{
	Mat rAxis = center - eye;
	Mat cmat = (Mat_<double>(3, 3) <<
		0, -rAxis.at<double>(2, 0), rAxis.at<double>(1, 0),
		rAxis.at<double>(2, 0), 0, -rAxis.at<double>(0, 0),
		-rAxis.at<double>(1, 0), rAxis.at<double>(0, 0), 0);
	angle = (CV_PI / 180) * angle;
	Mat yawingMat = Mat::eye(3, 3, CV_64F) + sin(angle) * cmat + (1 - cos(angle)) * cmat * cmat;
	rmat = yawingMat * rmat;
}

void Camera::resetRmat()
{
	rmat = Mat::eye(3, 3, CV_64F);
}
void Camera::translate(double x, double y, double z)
{
	eye.at<double>(0, 0) += x;
	eye.at<double>(1, 0) += y;
	eye.at<double>(2, 0) += z;
	center.at<double>(0, 0) += x;
	center.at<double>(1, 0) += y;
	center.at<double>(2, 0) += z;
}

void Camera::update()
{
	center = rmat * (center - eye) + eye;
	up = rmat * up;
}

GLfloat eye[3], up[3], center[3];

double height = 10;
double width = 30;
GLfloat uav_pos[3] = { 0, 0, 0 };
Mat uav_rvec(3, 1, CV_64F), uav_rmat(3, 3, CV_64F);
GLdouble uav[][3] = {
	{ 0.0, 0.0, 0.0 },
	{ width, 0.0, 0.0 },
	{ width, width, 0.0 },
	{ 0.0, width, 0.0 },
	{ 0.0, 0.0, height },
	{ width, 0.0, height },
	{ width, width, height },
	{ 0.0, width, height }
};
int face[][4] = {//�ʂ̒�`
	{ 0, 1, 2, 3 },
	{ 1, 5, 6, 2 },
	{ 5, 4, 7, 6 },
	{ 4, 0, 3, 7 },
	{ 4, 5, 1, 0 },
	{ 3, 2, 6, 7 }
};
void draw(void)
{
	const float side = 30;
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			glBegin(GL_QUADS);
			if (i % 2 == 0)
			{
				if (j % 2 == 0)
					glColor4d(1, 1, 1, 0);
				else
					glColor4d(0, 0, 0, 0);
			}
			else
			{
				if (j % 2 == 0)
					glColor4d(0, 0, 0, 0);
				else
					glColor4d(1, 1, 1, 0);
			}
			glVertex3f(side * j, side * i, 0);
			glVertex3f(side * j + side - 1, side * i, 0);
			glVertex3f(side * j + side - 1, side * i + side - 1, 0);
			glVertex3f(side * j, side * i + side - 1, 0);
			glEnd();
		}
	}
	glColor4d(1, 0, 0, 1);
	line3D(0, 0, 0, 1000, 0, 0);
	glColor4d(0, 1, 0, 1);
	line3D(0, 0, 0, 0, 1000, 0);
	glColor4d(0, 0, 1, 1);
	line3D(0, 0, 0, 0, 0, 1000);

	//glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

	rotate_uav();
	glPushMatrix();
	glColor3d(0.0, 1.0, 1.0);//�F�̐ݒ�
	glTranslated(uav_pos[0], uav_pos[1], uav_pos[2]);//���s�ړ��l�̐ݒ�
	glBegin(GL_QUADS);
	for (int j = 0; j < 6; ++j) {
		for (int i = 0; i < 4; ++i) {
			glVertex3dv(uav[face[j][i]]);
		}
	}
	glEnd();
	glPopMatrix();
	


}

Camera cam(0, 0, 100);

char buf[64];
int n;
struct sockaddr_in server;
int sock;
void display()
{
	memset(buf, 0, sizeof(buf));
	n = recv(sock, buf, sizeof(buf), 0);
	if (n == SOCKET_ERROR)
	{
		cerr << "Error: socket communication not well." << endl;
		exit(EXIT_FAILURE);
	}
	cout << buf << endl;
	char *value = strtok(buf, " ");
	for (int i = 0; i < 6; ++i)
	{
		if (i < 3)
			uav_pos[i] = atof(value);
		else
			uav_rvec.at<double>(i - 3, 0) = atof(value);
		cout << value << endl;
		value = strtok(NULL, " ");
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (double)WIDTH / (double)HEIGHT, 0.5, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam.eye.at<double>(0, 0), cam.eye.at<double>(1, 0), cam.eye.at<double>(2, 0),
		cam.center.at<double>(0, 0), cam.center.at<double>(1, 0), cam.center.at<double>(2, 0),
		cam.up.at<double>(0, 0), cam.up.at<double>(1, 0), cam.up.at<double>(2, 0));
	draw();
	glutSwapBuffers();
	glFlush();
}

GLdouble temp[3];
void rotate_uav()
{

	Rodrigues(uav_rvec, uav_rmat);
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			temp[j] = uav_rmat.at<double>(j, 0) * uav[i][0]
				+ uav_rmat.at<double>(j, 1) * uav[i][1]
				+ uav_rmat.at<double>(j, 2) * uav[i][2];
		}
		memcpy(uav[i], temp, sizeof(temp));
	}
}

void init()
{

	WSADATA data;
	WSAStartup(MAKEWORD(2, 0), &data);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	server.sin_family = AF_INET;
	server.sin_port = htons(12345);
	server.sin_addr.s_addr = inet_addr("172.18.1.10");

	connect(sock, (struct sockaddr *)&server, sizeof(server));
	
	glClearColor(0.3, 0.3, 0.3, 0);
	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);*/
}

void specialKeyboard(int key, int x, int y)
{
	cam.resetRmat();
	switch (key)
	{
	case GLUT_KEY_LEFT:
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			cam.yaw(-1);
			break;
		}
		cam.roll(1);
		break;
	case GLUT_KEY_RIGHT:
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			cam.yaw(1);
			break;
		}
		cam.roll(-1);
		break;
	case GLUT_KEY_UP:
		cam.pitch(1);
		break;
	case GLUT_KEY_DOWN:
		cam.pitch(-1);
		break;
	default:
		break;
	}
	cam.update();
}

void keyboard(unsigned char key, int x, int y)
{
	double temp[3] = { cam.center.at<double>(0, 0) - cam.eye.at<double>(0, 0),
		cam.center.at<double>(1, 0) - cam.eye.at<double>(1, 0),
		cam.center.at<double>(2, 0) - cam.eye.at<double>(2, 0) };
	const int speed = 3;
	switch (key)
	{
	case '[':
		cam.translate(speed * temp[0], speed * temp[1], speed * temp[2]);
		break;
	case ']':
		cam.translate(-speed * temp[0], -speed * temp[1], -speed * temp[2]);
		break;
	case 'w':
		cam.translate(speed * cam.up.at<double>(0, 0),
			speed * cam.up.at<double>(1, 0),
			speed * cam.up.at<double>(2, 0));
		break;
	case 's':
		cam.translate(-speed * cam.up.at<double>(0, 0),
			-speed * cam.up.at<double>(1, 0),
			-speed * cam.up.at<double>(2, 0));
		break;
	case 'd':
		cam.translate(speed * (temp[1] * cam.up.at<double>(2, 0) - temp[2] * cam.up.at<double>(1, 0)),
			speed * (temp[2] * cam.up.at<double>(0, 0) - temp[0] * cam.up.at<double>(2, 0)),
			speed * (temp[0] * cam.up.at<double>(1, 0) - temp[1] * cam.up.at<double>(0, 0)));
		break;
	case 'a':
		cam.translate(speed * (-temp[1] * cam.up.at<double>(2, 0) + temp[2] * cam.up.at<double>(1, 0)),
			speed * (-temp[2] * cam.up.at<double>(0, 0) + temp[0] * cam.up.at<double>(2, 0)),
			speed * (-temp[0] * cam.up.at<double>(1, 0) + temp[1] * cam.up.at<double>(0, 0)));
		break;
	}
}

void postProcess()
{
	closesocket(sock);
	WSACleanup();
}

void timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(100, timer, 0);
}