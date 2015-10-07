#include <fstream>
#include <time.h>
#include <Windows.h>
#include <iostream>
#include <algorithm>
#include "Solvers/EllipsoidBuilders.h"
//#include "Helper.h"
#include "MouseHandler.h"

Window* window;

void glut_init(int argc, char *argv[], int window_width, int window_height, int window_pos_x, int window_pos_y);
void tmf(int value);
void Display();
void Reshape(int w, int h);

void setColor(vector<Point2D>* points, const Color color){
	for (int i = 0; i < points->size(); ++i)
	{
		points->at(i).SetColor(color);
	}
	return;
}

double maxX(vector<Point2D>* points){
	return max_element(points->begin(), points->end(), [](Point2D a, Point2D b) { return a.X() < b.X(); })->X();
}

double maxY(vector<Point2D>* points){
	return max_element(points->begin(), points->end(), [](Point2D a, Point2D b) { return a.Y() < b.Y(); })->Y();
}

double minX(vector<Point2D>* points){
	return min_element(points->begin(), points->end(), [](Point2D a, Point2D b) { return a.X() < b.X(); })->X();
}

double minY(vector<Point2D>* points){
	return min_element(points->begin(), points->end(), [](Point2D a, Point2D b) { return a.Y() < b.Y(); })->Y();
}

void shiftPoints(vector<Point2D>* points, const double & dx, const double & dy){
	for (int i = 0; i < points->size(); ++i){
		points->at(i) = Point2D(points->at(i).X() + dx, points->at(i).Y() + dy);
	}
}

void scalePoints(vector<Point2D>* points, const double & xCoef, const double & yCoef){
	for (int i = 0; i < points->size(); ++i){
		points->at(i) = Point2D(points->at(i).X() * xCoef, points->at(i).Y() * yCoef);
	}
}

void normalizePoints(vector<Point2D>* windowVertexes, vector<Point2D>* allVertexes){
	double dX = -minX(allVertexes);
	double dY = -minY(allVertexes);
	shiftPoints(windowVertexes, dX, dY);
	shiftPoints(allVertexes, dX, dY);
	double xCoef = 950 / maxX(allVertexes);
	double yCoef = 400 / maxY(allVertexes);
	scalePoints(windowVertexes, xCoef, yCoef);
	scalePoints(allVertexes, xCoef, yCoef);
	shiftPoints(windowVertexes, 200, 200);
	shiftPoints(allVertexes, 200, 200);
}

void writeToFile(vector<Point2D>* vertexes, string file){
	ofstream fo("result.txt", ios::trunc);
	fo << "<<<<<<<<<Minimal volume ellipsoid>>>>>>>>>>>>>>\n";
	fo.close();
	IEllipsoidBuilder* rublevBuilder = new RublevEllipsoidBuilder();
	Ellipsoid2D* el = rublevBuilder->Exec(*window->vertexes);
	MouseHandler::WriteResultsToFile("../../EMAlgo/Results/" + file + ".txt", "Rublev", el);
}

void Keyboard(unsigned char key, int x, int y);

void main(int argc, char *argv[])
{
	srand(time(NULL));
	glut_init(argc, argv, 1350, 700, 0, 0);
	glutCreateWindow("Ellipse of minimal volume");

	///when start press Enter to view next result

	vector<string> listOfFiles = { "dataset_test", "dataset1", "dataset2", "dataset3", "faithful (1)", "faithful (2)", "faithful", "input_-_kopia", "points_5", "points_7", "points_9" };

	for (auto i = 0; i < listOfFiles.size(); ++i)
	{
		//read vertexes from file
		window->vertexes = Helper::ReadPoints("../../EMAlgo/Input/" + listOfFiles[i] + ".txt");
		//window->vertexes = Helper::ReadPoints("Resources/points_7.txt");
		writeToFile(window->vertexes, listOfFiles[i]);
		vector<Point2D>* allVertexes = Helper::ReadPoints("../../EMAlgo/Prepared\ Data/" + listOfFiles[i] + ".txt");
		//vector<Point2D>* allVertexes = Helper::ReadPoints("Resources/points_7.txt");
		//normalize points and set color for drawing
		normalizePoints(window->vertexes, allVertexes);
		setColor(allVertexes, Color(0.0, 0.0, 0.0));
		//finding ellipse
		IEllipsoidBuilder* rublevBuilder = new RublevEllipsoidBuilder();
		Ellipsoid2D* el = rublevBuilder->Exec(*window->vertexes);
		//drawing ellipse
		DrawEvent* draw = new DrawEvent();
		draw->add(allVertexes);
		draw->add(window->vertexes);
		draw->add(1, el);
		window->AddNewDrawEvent(draw);
	}

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(window->rendering, tmf, 0);
	glutMainLoop();
}

void Keyboard(unsigned char key, int x, int y)
{
	#define ESCAPE 27
	#define SPACE 32
	#define ENTER 13

		switch (key)
		{
		case ESCAPE:
			if (window != NULL)
			{
				delete window;
				window = NULL;
			}
			exit(0);
			break;
		case SPACE://pause
			if (window != NULL) window->pause = !window->pause;
			break;
		case ENTER://next move
			if (window->DrawEvents != NULL)
			{
				if (!window->DrawEvents->empty())
				{
					delete window->CurrentWindowContent;
					window->CurrentWindowContent = window->FirstDrawEvent();
				}
				if (window->NumberOfDrawEvents() > 1)
					window->PopDrawEvent();
			}
			break;
		}
};

void glut_init(int argc, char *argv[], int window_width, int window_height, int window_pos_x, int window_pos_y)
{
	window = Window::Create(window_width, window_height);
	window->pause = true;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(window_pos_x, window_pos_y);
	return;
}

void tmf(int value) // Timer function
{
	if (window == NULL) return;
	if (!window->pause)
	{	
		if (window->DrawEvents != NULL) 
		{
			if(!window->DrawEvents->empty())
			{
				delete window->CurrentWindowContent;
				window->CurrentWindowContent = window->FirstDrawEvent();
			}
			if (window->NumberOfDrawEvents() > 1)
				window->PopDrawEvent();
		}
	}
	glutPostRedisplay();// Redraw windows
	glutTimerFunc(window->rendering, tmf, 0);// Setup next timer
}

void Display()
{
	glClearColor(window->BackColor.RED, window->BackColor.GREEN, window->BackColor.BLUE, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glLineWidth(1.0);
	if (window != NULL)
	{
		if (window->CurrentWindowContent != NULL) window->CurrentWindowContent->Draw();
		if (window->vertexes != NULL && window->DrawVertexes)
		{
			//for (int i = 0; i < window->vertexes->size(); ++i)
			//	window->vertexes->at(i).Draw();
		}
	}
	glFlush();
	glutSwapBuffers();
};

void Reshape(int w, int h)
{
	if (window == NULL) window = Window::Create(w, h);
	window->Width = w;
	window->Height = h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
};