#include <time.h>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <list>
#include <GL/glut.h>
#include<GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
using namespace std;
class Dot
{
private:
	int newX;
	int newY;
	float newR;
	float newG;
	float newB;
public:
	Dot() {}
	Dot(int x, int y, float r, float g, float b) {
		newX = x;
		newY = y;
		newR = r;
		newG = g;
		newB = b;
	}
	~Dot() {}
	int getX() {
		return newX;
	}
	int getY() {
		return newY;
	}
	float getR() {
		return newR;
	}
	float getG() {
		return newG;
	}
	float getB() {
		return newB;
	}
	void setPosition(int x, int y) {
		newX = x;
		newY = y;
	}
	void setColour(float r, float g, float b) {
		newR = r;
		newG = g;
		newB = b;
	}
};
using namespace std;
float red = 0.0, green = 0.0, blue = 0.0;
int tmpx, tmpy;
int brushSize = 4;
int eraserSize = 1;
bool isSecond = false;
bool isRandom = false;
bool isEraser = false;
bool isRadial = false;
bool first = true;
float window_w_max = 1366;
float window_h_max = 768;
float window_w = 3 * (window_w_max / 4);
float window_h = 3 * (window_h_max / 4);
int shape = 1;
vector<Dot> dots;		// store all the points until clear
list<int> undoHistory; // record for undo, maximum 20 shapes in history
list<int> redoHistory; // record for redo, maximum 20 shapes in history
vector<Dot> redoDots;  // store the dots after undo temporaly
void canvas();
void display(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(2);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < dots.size(); i++)
	{
		glColor3f(dots[i].getR(), dots[i].getG(), dots[i].getB());
		glVertex2i(dots[i].getX(), dots[i].getY());
	}
	glEnd();
	canvas();
	glutSwapBuffers();
	glutPostRedisplay();
}

void clear()
{
	first = true;
	dots.clear();
	undoHistory.clear();
	redoDots.clear();
	redoHistory.clear();
	glClear(GL_COLOR_BUFFER_BIT);
	//glutSwapBuffers();
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	cout << "[Info] The window and the history are cleared successfully. " << asctime(timeinfo) << endl;
}

void quit()
{
	cout << "Thank you for using this Paint tool! Goodbye!" << endl;
	exit(0);
}

void undo()
{
	if (undoHistory.size() == 0)
	{
		undoHistory.size() + 1;
	}
	if (undoHistory.size() > 0)
	{
		int numRemove = dots.size() - undoHistory.back();
		for (int i = 0; i < numRemove; i++)
		{
			redoDots.push_back(dots.back());
			dots.pop_back();
		}
		redoHistory.push_back(undoHistory.back());
		undoHistory.pop_back();
	}
	else
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
			<< "[Warning] Cannot undo. This is the first record in the history.\n";
	}
}
void redo()
{
	if (redoHistory.size() > 1)
	{
		undoHistory.push_back(redoHistory.back());
		redoHistory.pop_back();
		int numRemove = redoHistory.back() - dots.size();
		for (int i = 0; i < numRemove; i++)
		{
			dots.push_back(redoDots.back());
			redoDots.pop_back();
		}
	}
	else
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
			<< "[Warning] Cannot redo. This is the last record in the history.\n";
	}
}

void drawDot(int mousex, int mousey)
{
	if (mousex >= window_w_max / 5 && mousex <= 4 * (window_w_max / 5))
	{
		Dot newDot(mousex, window_h - mousey, isEraser ? 1.0 : red, isEraser ? 1.0 : green, isEraser ? 1.0 : blue);
		dots.push_back(newDot);
	}
}
void drawDot_max(int mousex, int mousey)
{
	Dot newDot(mousex, window_h - mousey, isEraser ? 1.0 : red, isEraser ? 1.0 : green, isEraser ? 1.0 : blue);
	dots.push_back(newDot);
}

void drawLine(int x1, int y1, int x2, int y2)
{
	bool changed = false;
	// Bresenham's line algorithm is only good when abs(dx) >= abs(dy)
	// So when abs(dx) < abs(dy), change axis x and y
	if (abs(x2 - x1) < abs(y2 - y1))
	{
		int tmp1 = x1;
		x1 = y1;
		y1 = tmp1;
		int tmp2 = x2;
		x2 = y2;
		y2 = tmp2;
		changed = true;
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	int xi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}
	int d = 2 * dy - dx;
	int incrE = dy * 2;
	int incrNE = 2 * dy - 2 * dx;

	int x = x1, y = y1;
	if (changed)
		drawDot(y, x);
	else
		drawDot(x, y);
	while (x != x2)
	{
		if (d <= 0)
			d += incrE;
		else
		{
			d += incrNE;
			y += yi;
		}
		x += xi;
		if (changed)
			drawDot(y, x);
		else
			drawDot(x, y);
	}
}
void drawLine_max(int x1, int y1, int x2, int y2)
{
	bool changed = false;
	// Bresenham's line algorithm is only good when abs(dx) >= abs(dy)
	// So when abs(dx) < abs(dy), change axis x and y
	if (abs(x2 - x1) < abs(y2 - y1))
	{
		int tmp1 = x1;
		x1 = y1;
		y1 = tmp1;
		int tmp2 = x2;
		x2 = y2;
		y2 = tmp2;
		changed = true;
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	int xi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}
	int d = 2 * dy - dx;
	int incrE = dy * 2;
	int incrNE = 2 * dy - 2 * dx;

	int x = x1, y = y1;
	if (changed)
		drawDot_max(y, x);
	else
		drawDot_max(x, y);
	while (x != x2)
	{
		if (d <= 0)
			d += incrE;
		else
		{
			d += incrNE;
			y += yi;
		}
		x += xi;
		if (changed)
			drawDot_max(y, x);
		else
			drawDot_max(x, y);
	}
}
/**
 * We can use drawLine function to draw the rectangle
 *
 * Top-left corner specified by the first click,
 * and the bottom-right corner specified by a second click
 */
void drawRectangle(int x1, int y1, int x2, int y2)
{
	if (x1 < x2 && y1 < y2)
	{
		drawLine(x1, y1, x2, y1);
		drawLine(x2, y1, x2, y2);
		drawLine(x2, y2, x1, y2);
		drawLine(x1, y2, x1, y1);
	}
	else
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
			<< "[Warning] The first click should be the top-left corner, the second click should be bottom-right corner.\n";
	}
}
void drawRectangle_max(int x1, int y1, int x2, int y2)
{
	if (x1 < x2 && y1 < y2)
	{
		drawLine_max(x1, y1, x2, y1);
		drawLine_max(x2, y1, x2, y2);
		drawLine_max(x2, y2, x1, y2);
		drawLine_max(x1, y2, x1, y1);
	}
	else
	{
		time_t rawtime;
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		std::cout << asctime(timeinfo)
			<< "[Warning] The first click should be the top-left corner, the second click should be bottom-right corner.\n";
	}
}
/**
 * Midpoint circle algorithm
 */

void drawCircle(int x1, int y1, int x2, int y2)
{
	
	int r = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	double d;
	int x, y;

	x = 0;
	y = r;
	d = 1.25 - r;

	while (x <= y)
	{
		drawDot(x1 + x, y1 + y);
		drawDot(x1 - x, y1 + y);
		drawDot(x1 + x, y1 - y);
		drawDot(x1 - x, y1 - y);
		drawDot(x1 + y, y1 + x);
		drawDot(x1 - y, y1 + x);
		drawDot(x1 + y, y1 - x);
		drawDot(x1 - y, y1 - x);
		x++;
		if (d < 0)
		{
			d += 2 * x + 3;
		}
		else
		{
			y--;
			d += 2 * (x - y) + 5;
		}
	}
}
void drawCircle_max(int x1, int y1, int x2, int y2)
{
	int r = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	double d;
	int x, y;

	x = 0;
	y = r;
	d = 1.25 - r;

	while (x <= y)
	{
		drawDot_max(x1 + x, y1 + y);
		drawDot_max(x1 - x, y1 + y);
		drawDot_max(x1 + x, y1 - y);
		drawDot_max(x1 - x, y1 - y);
		drawDot_max(x1 + y, y1 + x);
		drawDot_max(x1 - y, y1 + x);
		drawDot_max(x1 + y, y1 - x);
		drawDot_max(x1 - y, y1 - x);
		x++;
		if (d < 0)
		{
			d += 2 * x + 3;
		}
		else
		{
			y--;
			d += 2 * (x - y) + 5;
		}
	}
}
void drawBrush(int x, int y)
{
	for (int i = 0; i < brushSize; i++)
	{
		int randX = rand() % (brushSize + 1) - brushSize / 2 + x;
		int randY = rand() % (brushSize + 1) - brushSize / 2 + y;
		drawDot(randX, randY);
	}
}
void drawRadialBrush(int x, int y)
{
	int xc = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int yc = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	int dx, dy;

	dx = xc - x + 100;
	dy = yc - y + 100;

	drawDot(xc + dx, yc + dy);
	drawDot(xc - dx, yc + dy);
	drawDot(xc + dx, yc - dy);
	drawDot(xc - dx, yc - dy);
	drawDot(xc + dy, yc + dx);
	drawDot(xc - dy, yc + dx);
	drawDot(xc + dy, yc - dx);
	drawDot(xc - dy, yc - dx);
}

void erase(int x, int y)
{
	for (int i = -eraserSize; i <= eraserSize; i++)
	{
		for (int j = -eraserSize; j <= eraserSize; j++)
		{
			drawDot(x + i, y + j);
		}
	}
}

void keyboard(unsigned char key, int xIn, int yIn)
{
	isSecond = false;
	switch (key)
	{
	case 'q':
	case 27: // 27 is the esc key
		quit();
		break;
	case 'c':
		clear();
		break;
	case '+':
		if (shape == 5 && !isEraser)
		{
			if (brushSize < 16)
				brushSize += 4;
			else
			{
				time_t rawtime;
				struct tm* timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
					<< "[Warning] Airbrush's size cannot be larger. It is already the largest.\n";
			}
		}
		else if (isEraser)
		{
			if (eraserSize < 10)
				eraserSize += 4;
			else
			{
				time_t rawtime;
				struct tm* timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
					<< "[Warning] Eraser's size cannot be larger. It is already the largest.\n";
			}
		}
		break;
	case '-':
		if (shape == 5 && !isEraser)
		{
			if (brushSize > 4)
				brushSize -= 4;
			else
			{
				time_t rawtime;
				struct tm* timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
					<< "[Warning] Airbrush's size cannot be smaller. It is already the smallest.\n";
			}
		}
		else if (isEraser)
		{
			if (eraserSize > 2)
				eraserSize -= 4;
			else
			{
				time_t rawtime;
				struct tm* timeinfo;
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				std::cout << asctime(timeinfo)
					<< "[Warning] Eraser's size cannot be smaller. It is already the smallest.\n";
			}
		}
		break;
	case 'u':
		undo();
		break;
	case 'r':
		redo();
		break;
	}
}
void motion(int x, int y)
{
	if (isEraser)
		erase(x, y);
	else
	{
		if (shape == 1)
		{
			if (isRadial)
				drawRadialBrush(x, y);
			else
				drawDot(x, y);
		}
		if (shape == 5)
			drawBrush(x, y);
	}
}

void reshape(int w, int h)
{
	window_w = w;
	window_h = h;
	window_w_max = w;
	window_h_max = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	clear();
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void processMainMenu(int value)
{
	switch (value)
	{
	case 0:
		quit();
		break;
	case 1:
		clear();
		break;
	case 2:
		undo();
		break;
	case 3:
		redo();
		break;
	}
}

void processBrushSizeMenu(int value)
{
	shape = 5;
	isEraser = false;
	brushSize = value;
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);
}

void processColourMenu(int value)
{
	isSecond = false;
	isEraser = false;
	isRandom = false;

	switch (value)
	{
	case 1: // red
		red = 1.0;
		green = 0.0;
		blue = 0.0;
		break;
	case 2: // blue
		red = 0.0;
		green = 0.0;
		blue = 1.0;
		break;
	case 3: // green
		red = 0.0;
		green = 1.0;
		blue = 0.0;
		break;
	case 4: // yellow
		red = 1.0;
		green = 1.0;
		blue = 0.0;
		break;
	case 5: // purple
		red = 0.5;
		green = 0.0;
		blue = 0.5;
		break;
	case 6: // brown
		red = 0.5;
		green = 0;
		blue = 0;
		break;
	case 7: // Cyan
		red = 0;
		green = 100;
		blue = 100;
		break;
	case 8: // Orange
		red = 1;
		green = 0.5;
		blue = 0;
		break;
	case 9: // Grey
		red = 0.5;
		green = 0.5;
		blue = 0.5;
		break;
	case 0: // Black
		red = 0;
		green = 0;
		blue = 0;
		break;
	case 10:
		isRandom = true;
		break;

	}
}

void processShapeMenu(int value)
{
	shape = value;
	isEraser = false;
	isSecond = false;
	isRadial = false;

	switch (shape)
	{
	case 1:
		glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
		break;
	case 2:
	case 3:
	case 4:
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		break;
	}
}

void processEraserSizeMenu(int value)
{
	glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
	eraserSize = value;
	isEraser = true;
}

void processRadicalBrushMenu(int value)
{
	isRadial = value == 1 ? true : false;
}

void createOurMenu()
{
	int colourMenu = glutCreateMenu(processColourMenu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Purple", 4);
	glutAddMenuEntry("Yellow", 5);
	glutAddMenuEntry("Random", 6);

	int sizeMenu = glutCreateMenu(processBrushSizeMenu);
	glutAddMenuEntry("4px", 4);
	glutAddMenuEntry("8px", 8);
	glutAddMenuEntry("12px", 12);
	glutAddMenuEntry("16px", 16);

	int shapeMenu = glutCreateMenu(processShapeMenu);
	glutAddMenuEntry("Point", 1);
	glutAddMenuEntry("Line", 2);
	glutAddMenuEntry("Rectangle", 3);
	glutAddMenuEntry("Circle", 4);
	glutAddSubMenu("Airbrush", sizeMenu);

	int eraserSizeMenu = glutCreateMenu(processEraserSizeMenu);
	glutAddMenuEntry("Small", 2);
	glutAddMenuEntry("Medium", 6);
	glutAddMenuEntry("Large", 10);

	int radicalBrushMenu = glutCreateMenu(processRadicalBrushMenu);
	glutAddMenuEntry("True", 1);
	glutAddMenuEntry("False", 2);

	int main_id = glutCreateMenu(processMainMenu);
	glutAddSubMenu("Colour", colourMenu);
	glutAddSubMenu("Shapes", shapeMenu);
	glutAddSubMenu("Radical Paint Brush", radicalBrushMenu);
	glutAddSubMenu("Eraser", eraserSizeMenu);
	glutAddMenuEntry("Save", 4);
	glutAddMenuEntry("Undo", 2);
	glutAddMenuEntry("Redo", 3);
	glutAddMenuEntry("Clear", 1);
	glutAddMenuEntry("Quit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}




void mouse(int bin, int state, int x, int y)
{
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (isRandom)
		{
			srand(time(NULL));
			red = float(rand()) / float(RAND_MAX);
			green = float(rand()) / float(RAND_MAX);
			blue = float(rand()) / float(RAND_MAX);
		}
		if (isEraser)
		{
			undoHistory.push_back(dots.size());
			erase(x, y);
		}
		else
		{
			if (shape == 1)
			{
				undoHistory.push_back(dots.size());
				if (isRadial)
					drawRadialBrush(x, y);
				else
					drawDot(x, y);
			}
			else if (shape == 5)
			{
				undoHistory.push_back(dots.size());
				drawBrush(x, y);
			}
			else
			{
				if (!isSecond)
				{
					tmpx = x;
					tmpy = y;
					isSecond = true;
				}
				else
				{
					if (undoHistory.back() != dots.size())
						undoHistory.push_back(dots.size());
					if (shape == 2)
						drawLine(tmpx, tmpy, x, y);
					else if (shape == 3)
						drawRectangle(tmpx, tmpy, x, y);
					else if (shape == 4)
						drawCircle(tmpx, tmpy, x, y);
					isSecond = false;
				}
			}
		}
		if (x > 59 && y > 59 && x < 124 && y < 105)
		{
			processColourMenu(1);
		}
		if (x > 147 && y > 59 && x < 216 && y < 105)
		{
			processColourMenu(2);
		}
		if (x > 59 && y > 118 && x < 124 && y < 160)
		{
			processColourMenu(3);
		}
		if (x > 147 && y > 118 && x < 216 && y < 160)
		{
			processColourMenu(4);
		}
		if (x > 59 && y > 174 && x < 124 && y < 216)
		{
			processColourMenu(5);
		}
		if (x > 147 && y > 174 && x < 216 && y < 216)
		{
			processColourMenu(6);
		}
		if (x > 59 && y > 232 && x < 124 && y < 273)
		{
			processColourMenu(7);
		}
		if (x > 147 && y > 232 && x < 216 && y < 273)
		{
			processColourMenu(8);
		}
		if (x > 59 && y > 289 && x < 124 && y < 333)
		{
			processColourMenu(9);
		}
		if (x > 147 && y > 289 && x < 216 && y < 333)
		{
			processColourMenu(0);
		}
		if (x > 152 && y > 431 && x < 211 && y < 450)
		{
			processShapeMenu(3);
		}
		if (x > 91 && y > 440 && x < 116 && y < 455.5)
		{
			processShapeMenu(4);
		}
		if (undoHistory.size() > 20)
		{
			undoHistory.pop_front();
		}

	}
}

void init(void)
{
	/* background color */
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor3f(red, green, blue);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, window_w, 0.0, window_h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void canvas() {
	if (first == true)
	{
		for (int i = 0; i != 3; i++) {//canvas
			drawRectangle_max(i, i, (window_w_max / 5) - i, window_w_max - i);
			drawRectangle_max(4 * (window_w_max / 5) - i, i, window_w_max - i, window_h_max - i);
		}
		drawRectangle_max(1 + (window_h_max / 15), 1 + (window_h_max / 15), (window_w_max / 6) - 1, (window_w_max / 4) - 1);
		drawRectangle_max((window_h_max / 15), (window_h_max / 15), (window_w_max / 6), (window_w_max / 4));
		for (int i = 0; i != 60; i++)
		{
			red = 0; green = 0; blue = 0;
			drawRectangle_max(i + (window_h_max / 15), i + (window_h_max / 2), (window_w_max / 6) - i, (window_w_max / 2.8) - i);
		}
		for (int i = 0; i != 37; i++) {//border
			red = 1; green = 1; blue = 1;
			drawRectangle_max(i + (window_h_max / 13), i + (window_h_max / 1.9), (window_w_max / 11) - i, (window_w_max / 2.9) - i);
			drawRectangle_max(i + (window_h_max / 5.2), i + (window_h_max / 1.9), (window_w_max / 6.3) - i, (window_w_max / 2.9) - i);
		}
		red = 0; green = 0; blue = 0;
		drawRectangle_max(5 + (window_h_max / 5.2), 5 + (window_h_max / 1.8), (window_w_max / 6.3) - 5, (window_w_max / 3) - 5);
		drawCircle_max(((window_h_max / 13) + (window_w_max / 11)) / 2, ((5 + (window_h_max / 1.8)) + ((window_w_max / 3) - 5)) / 2, (((window_h_max / 13) + (window_w_max / 11)) / 2) + 25, (((25 + (window_h_max / 1.8)) + ((window_w_max / 3) - 5)) / 2) + 5);
		//clrs
		for (int i = 0; i != 25; i++)//clr sides
		{
			red = 1; green = 0; blue = 0;
			drawRectangle_max(i + (window_h_max / 13), i + (window_h_max / 13), (window_w_max / 11) - i, (window_w_max / 13) - i);
			red = 0; green = 0; blue = 1;
			drawRectangle_max(i + (window_h_max / 5.2), i + (window_h_max / 13), (window_w_max / 6.3) - i, (window_w_max / 13) - i);
		}
		for (int i = 0; i != 25; i++) {
			red = 0; green = 1; blue = 0;
			drawRectangle_max(i + (window_h_max / 13), i + (window_h_max / 6.5), (window_w_max / 11) - i, (window_w_max / 8.5) - i);
			red = 255; green = 255;
			drawRectangle_max(i + (window_h_max / 5.2), i + (window_h_max / 6.5), (window_w_max / 6.3) - i, (window_w_max / 8.5) - i);
		}
		for (int i = 0; i != 25; i++) {
			red = 255; green = 0; blue = 255;
			drawRectangle_max(i + (window_h_max / 13), i + (window_h_max / 4.4), (window_w_max / 11) - i, (window_w_max / 6.3) - i);
			red = 0.5; green = 0; blue = 0;
			drawRectangle_max(i + (window_h_max / 5.2), i + (window_h_max / 4.4), (window_w_max / 6.3) - i, (window_w_max / 6.3) - i);
		}
		for (int i = 0; i != 25; i++) {
			red = 0.6; green = 0.8; blue = 0.9;
			drawRectangle_max(i + (window_h_max / 13), i + (window_h_max / 3.3), (window_w_max / 11) - i, (window_w_max / 5) - i);
			red = 1; green = 0.5; blue = 0;
			drawRectangle_max(i + (window_h_max / 5.2), i + (window_h_max / 3.3), (window_w_max / 6.3) - i, (window_w_max / 5) - i);
		}
		for (int i = 0; i != 30; i++)
		{
			red = 0.5; green = 0.5; blue = 0.5;
			drawRectangle_max(i + (window_h_max / 13), i + (window_h_max / 2.65), (window_w_max / 11) - i, (window_w_max / 4.1) - i);
			red = 0; green = 0; blue = 0;
			drawRectangle_max(i + (window_h_max / 5.2), i + (window_h_max / 2.65), (window_w_max / 6.3) - i, (window_w_max / 4.1) - i);
		}
		first = false;
	}
}

void FPS(int val)
{
	glutPostRedisplay();
	glutTimerFunc(100, FPS, 0);
}

void callbackInit()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutTimerFunc(100, FPS, 0);
}


int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); //initial display mode in RGB Double Buffer
	glutInitWindowSize(window_w_max, window_h_max);       //initial window size
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Shity Paint");
	callbackInit();
	init();
	createOurMenu();
	glutMainLoop();
	return (0);
}