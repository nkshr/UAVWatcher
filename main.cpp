#include "uav_watcher.h"

using namespace std;
using namespace cv;

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("display");
	glutDisplayFunc(display);
	glutTimerFunc(100, timer, NULL);
	glutSpecialFunc(specialKeyboard);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	postProcess();
	return 0;
}