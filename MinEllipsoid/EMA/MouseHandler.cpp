#include "MouseHandler.h"

const string MouseHandler::Rublev = "Rublev";

IEllipsoidBuilder* MouseHandler::GetCorrectBuilder(string nameOfAlgo)
{
	if (nameOfAlgo == Rublev)
		return new RublevEllipsoidBuilder();
}

void MouseHandler::Build2DEllipse(string nameOfAlgo, IEllipsoidBuilder* builder, bool toDraw)
{
	Window* window = Window::Create();
	if (toDraw == true && nameOfAlgo != Rublev) toDraw = false;

	if (window->vertexes->size() < 3) return;
	Ellipsoid2D* el;

	vector<Point2D>* conv = NULL;

	vector<Point2D> points;
	if (conv != NULL) points = (*conv);
	else points = *window->vertexes;

	if (toDraw) el = builder->Exec(points, window);
	else el = builder->Exec(points);

	DrawEvent* draw = new DrawEvent();
	draw->add(window->vertexes);
	if (el != NULL)
	{
		el->SetColor(Color(1.0f, 0.0f, 0.0f));
		draw->add(1, el);
	}
	window->AddNewDrawEvent(draw);
	delete draw; draw = NULL;

	WriteResultsToFile("Resources/Output Data.txt", nameOfAlgo, el);

	delete el; el = NULL;
}

void MouseHandler::WriteEllipse(ofstream & fo, string nameOfAlgo, const alglib::real_2d_array& eVectors, const alglib::real_1d_array& eValues, const int& dimension, const alglib::real_1d_array& centre)
{
	fo << ">>>" << nameOfAlgo << "<<<<\n";
	fo << "Parameters of the ellipsoid:\n";
	fo << "Centre of the ellipsoid:\n";
	fo << "(";
	for (int i = 0; i < dimension; ++i)
	{
		fo << centre[i];
		if (i != dimension - 1) fo << ", ";
	}
	fo << ")\n";
	fo << "Eigenvectors and Semi-axes:\n";
	for (int i = 0; i < dimension; ++i)
	{
		fo << "Eigenvector#" << i + 1 << ": (";
		for (int j = 0; j < dimension; ++j)
		{
			fo << eVectors(i, j);
			if (j != dimension - 1) fo << ", ";
		}
		fo << ") And the semi-axe which corresponds to this eigenvector: " << eValues[i] << "\n";
	}
}

void MouseHandler::ReadVertexesFromFile()
{
	Window* window = Window::Create();
	if (window->vertexes != NULL)
	{
		if (!window->vertexes->empty()) window->vertexes->clear();
		delete window->vertexes;
		window->vertexes = NULL;
	}
	window->clear();
	window->vertexes = Helper::ReadPoints("Resources/points_7.txt");
	DrawEvent* draw = new DrawEvent();
	draw->add(window->vertexes);
	window->AddNext(draw);
	delete draw;
	draw = NULL;
}

void MouseHandler::GenerateRandomVertexes()
{
	Window* window = Window::Create();
	if (window->vertexes != NULL)
	{
		if (!window->vertexes->empty()) window->vertexes->clear();
		delete window->vertexes;
		window->vertexes = NULL;
	}
	window->clear();
	window->vertexes = Helper::GenRandVertexes();
}

void MouseHandler::ProcessExit()
{
	Window* window = Window::Create();
	delete window;
	window = NULL;
	exit(0);
}

void MouseHandler::WriteResultsToFile(string nameOfFile, string nameOfAlgo, Ellipsoid2D* el, bool multiDim)
{
	Window* window = Window::Create();
	if (multiDim)
		Helper::WritePoints(*window->MultiDimVert, nameOfFile);
	else
		Helper::WritePoints(*window->vertexes, nameOfFile);

	ofstream fo(nameOfFile, ios::app);
	WriteEllipse(fo, nameOfAlgo, el->Eigenvectors(), el->Axes(), el->Dim(), el->Centre().Coords());
	if (!multiDim) fo << "Volume:\t" << el->Area() << endl;
	fo.close();
}

void MouseHandler::WriteResultsToFile(string nameOfFile, string nameOfAlgo, Ellipsoid* el, bool multiDim)
{
	Window* window = Window::Create();
	if (multiDim)
		Helper::WritePoints(*window->MultiDimVert, nameOfFile);
	else
		Helper::WritePoints(*window->vertexes, nameOfFile);

	ofstream fo(nameOfFile, ios::app);
	WriteEllipse(fo, nameOfAlgo, el->Eigenvectors(), el->Axes(), el->Dim(), el->Centre().Coords());
	fo.close();
}

void MouseHandler::BuildEllipsoid(string nameOfAlgo, bool multiDim, bool draw)
{
	window->clear();
	IEllipsoidBuilder* builder = MouseHandler::GetCorrectBuilder(nameOfAlgo);
	Build2DEllipse(nameOfAlgo, builder, draw);
	delete builder; builder = NULL;
}