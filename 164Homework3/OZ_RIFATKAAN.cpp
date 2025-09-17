/*********
   CTIS164 - Template Source Program
----------
STUDENT : Rifat Kaan Oz 21903166
SECTION :
HOMEWORK: Chasing the mouse pointer using Trigonometry and Vector Algebra
----------
PROBLEMS: When the object comes one of the edges, it stops, could not continue with a new angle
----------
ADDITIONAL FEATURES: When the object hits the mouse circle, it is randomly generated at a random point
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define SPEED 2

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

typedef struct {
	float xM, yM;
} point_tM;
point_tM mouse = { 1000,1000 };

typedef struct {
	vec_t pos;
	float angle;
	vec_t vel;  //velocity
	bool  visibility; // visibility: true if the player sees the enemy
} player_t;

player_t P = { { 0, 0 }, 90 };

float preX, preY;

bool mouseInside = false;
bool active = false;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char* string, void* font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void drawLabels() {

	//mouse coordinations to help designing
	//glColor3f(1, 1, 1);
	//vprint(330, 280, GLUT_BITMAP_8_BY_13, "%.0f %.0f", mouse.xM, mouse.yM);

	//stu info
	glColor3ub(153, 153, 255);
	vprint(-392, 280, GLUT_BITMAP_8_BY_13, "Rifat Kaan Oz 21903166");
	vprint(-392, 255, GLUT_BITMAP_8_BY_13, "CTIS 164 HW-3");

	//mouse inside or outside
	glColor3ub(255, 255, 255);
	vprint(-392, -280, GLUT_BITMAP_8_BY_13, "Mouse is ");
	if (mouse.xM > -320 && mouse.xM < 320 && mouse.yM<220 && mouse.yM>-220) {
		vprint(-392, -280, GLUT_BITMAP_8_BY_13, "         inside");
		mouseInside = true;
	}
	else
	{
		vprint(-392, -280, GLUT_BITMAP_8_BY_13, "         outside");
		mouseInside = false;
	}

	//pause-resume by spacebar info
	if (active == false) {
		vprint(-105, -280, GLUT_BITMAP_8_BY_13, "press spacebar to resume");
	}
	else {
		vprint(-105, -280, GLUT_BITMAP_8_BY_13, "press spacebar to pause");
	}

	//angle
	vprint(300, -280, GLUT_BITMAP_8_BY_13, "Angle = %0.f", P.angle);
	if (P.angle > 360) {
		P.angle -= 360;
	}
	if (P.angle < 0) {
		P.angle += 360;
	}
}

void drawMouseCircle() {

	if (mouseInside == true) {
		glColor3f(1, 0, 0);
		circle(mouse.xM, mouse.yM, 5);
	}
}

void drawObject(player_t p) {

	vec_t E1 = addV(p.pos, pol2rec({ 7, p.angle + 90 }));
	vec_t E2 = addV(p.pos, pol2rec({ 7, p.angle - 90 }));
	vec_t E3 = addV(p.pos, pol2rec({ 25, p.angle }));
	vec_t E4 = addV(p.pos, pol2rec({ 10, p.angle + 90 }));
	vec_t E5 = addV(p.pos, pol2rec({ 10, p.angle - 90 }));
	vec_t E6 = addV(p.pos, pol2rec({ 30, p.angle - 120 }));
	vec_t E7 = addV(p.pos, pol2rec({ 30, p.angle + 120 }));

	glColor3f(1, 0, 1);
	glBegin(GL_TRIANGLES);
	glVertex2f(E1.x, E1.y);
	glVertex2f(E2.x, E2.y);
	glVertex2f(E3.x, E3.y);
	glEnd();

	glColor3f(1, 0, 1);
	glBegin(GL_QUADS);
	glVertex2f(E4.x, E4.y);
	glVertex2f(E5.x, E5.y);
	glVertex2f(E6.x, E6.y);
	glVertex2f(E7.x, E7.y);
	glEnd();
}

void resetObject() {
	float xPos = rand() % 640 + 0;
	float yPos = rand() % 440 + 0;
	P.pos = { xPos - 320, yPos - 220 };
}
//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	//draw main area
	glColor3ub(204, 255, 204);
	glRectf(-320, -220, 320, 220);

	drawLabels();
	drawMouseCircle();
	drawObject(P);



	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ') {
		active = !active;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.
	mouse.xM = x - winWidth / 2;
	mouse.yM = winHeight / 2 - y;


	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	/*to control manually
	if (left) P.angle += 3;
	if (right) P.angle -= 3;
	if (up) P.pos = addV(P.pos, pol2rec({ SPEED, P.angle }));
	if (down) P.pos = subV(P.pos, pol2rec({ SPEED, P.angle }));*/

	vec_t prevPos = P.pos;

	P.vel = mulV(SPEED, unitV(subV({ mouse.xM,mouse.yM }, P.pos)));

	if (mouseInside == true && active == true) {
		P.angle = atan2(mouse.xM - P.pos.x, mouse.yM - P.pos.y) / D2R + 270;
		P.angle = 360 - P.angle;
		if (P.angle < 0) {
			P.angle += 360;
		}
		P.pos = addV(P.pos, P.vel);
		preX = mouse.xM;
		preY = mouse.yM;
	}
	if (mouseInside == false && active == true) {
		P.vel = mulV(SPEED, unitV(subV({ preX,preY }, P.pos)));
		P.pos = addV(P.pos, P.vel);
	}

	if (P.pos.x > 320 || P.pos.x < -320 || P.pos.y > 220 || P.pos.y < -220) {
		P.pos = prevPos;
	}

	if (fabs(P.pos.x - mouse.xM) < 2 && fabs(P.pos.y - mouse.yM) < 2) {
		resetObject();
	}


	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	srand(time(NULL));
	resetObject();
}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Template File");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}