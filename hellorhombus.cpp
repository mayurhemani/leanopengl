/**
* The very first program using GLEW and GLUT. Shows a rhombus.
*/
#include "GL/glew.h"
#include "GL/glut.h"


void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POLYGON);
		glVertex3f(0.0, -0.25, 0.0);
		glVertex3f(-0.25, 0.0, 0.0);
		glVertex3f(0.0, 0.25, 0.0);
		glVertex3f(0.25, 0.0, 0.0);
	glEnd();
	glFlush();
}


int main(int argc, char* argv[]) {
		
	// initialize glut
	glutInit(&argc, argv);
	// set single buffer display mode
	glutInitDisplayMode(GLUT_SINGLE);
	// create a simple 400x300 window
	glutInitWindowSize(400, 300);
	// move it to (100, 100)
	glutInitWindowPosition(100, 100);
	// create the window
	glutCreateWindow("hello world");

	// initialize OpenGL functions
	glewExperimental = true;
	glewInit();

	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}
