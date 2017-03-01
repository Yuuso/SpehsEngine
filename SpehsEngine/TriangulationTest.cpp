#include <algorithm>
#include <iostream>
#include <Windows.h>
#include "ApplicationData.h"
#include "InputManager.h"
#include "Geometry.h"
#include "Polygon.h"
#include "Arrow.h"
#include "Point.h"
#include "RNG.h"
#include "TriangulationTest.h"
#ifndef PI
#define PI 3.14159265359f
#endif
#ifndef TWO_PI
#define TWO_PI 6.28318530718f
#endif
namespace spehs
{
	std::atomic<int> triangleAllocations = 0;
	std::atomic<int> triangleDeallocations = 0;
	std::atomic<int> edgeAllocations = 0;
	std::atomic<int> edgeDeallocations = 0;
	int triangulationTestSeed = 0;
	int triangulationTestIteration = 0;

	DirectionalEdge::DirectionalEdge(glm::vec2* _begin, glm::vec2* _end, DirectionalEdge* _prev) : begin(_begin), end(_end), prev(nullptr), next(nullptr), innerTriangle(nullptr)
	{
		edgeAllocations++;
		setPrev(_prev);
	}
	DirectionalEdge::~DirectionalEdge()
	{
		edgeDeallocations++;
		if (prev && prev->next == this)
		{
			prev->next = nullptr;
			delete prev;
		}
		if (next && next->prev == this)
		{
			next->prev = nullptr;
			delete next;
		}
	}
	Triangle::Triangle(glm::vec2* p0, glm::vec2* p1, glm::vec2* p2, Triangle* _parent) : polygon(nullptr), parent(_parent)
	{
		triangleAllocations++;
		points[0] = p0;
		points[1] = p1;
		points[2] = p2;
		neighbours[0] = nullptr;
		neighbours[1] = nullptr;
		neighbours[2] = nullptr;
		if (parent)
			parent->children.push_back(this);

		//DEBUG
		neighbourArrows[0] = nullptr;
		neighbourArrows[1] = nullptr;
		neighbourArrows[2] = nullptr;
		parentArrow = nullptr;
	}
	Triangle::~Triangle()
	{
		triangleDeallocations++;
		polygon->destroy();
		for (unsigned i = 0; i < 3; i++)
		{
			if (neighbourArrows[i])
				delete neighbourArrows[i];
		}
		if (parentArrow)
			delete parentArrow;
	}
	bool Triangle::legalize()
	{
		//INITIAL CHECKING
		for (unsigned n = 0; n < 3; n++)
		{//For each neighbour

			if (neighbours[n])
			{
				//Check neighbour connection
				bool legit = false;
				for (int n2 = 0; n2 < 3; n2++)
				{
					if (neighbours[n]->neighbour(n2) == this)
					{
						legit = true;
						break;
					}
				}
				if (!legit)
					spehs::exceptions::unexpectedError("Neighbours not double linked!");

				//Check common corners
				int numCommonCorners = 0;
				for (int c1 = 0; c1 < 3; c1++)
				{
					for (int c2 = 0; c2 < 3; c2++)
					{
						if (point(c1) == neighbours[n]->point(c2))
						{
							numCommonCorners++;
							break;
						}
					}
				}
				if (numCommonCorners != 2)
					spehs::exceptions::unexpectedError("Neighbours do not have 2 common vertices!");

				//Check my corners
				legit = true;
				for (int c1 = 0; c1 < 3; c1++)
				{
					for (int c2 = c1 + 1; c2 < 3; c2++)
					{
						if (point(c1) == point(c2))
						{
							legit = false;
							break;
						}
					}
				}
				if (!legit)
					spehs::exceptions::unexpectedError("Triangle contains 2 or more of the same vertex!");
			}
		}


		for (unsigned n = 0; n < 3; n++)
		{//For each neighbour

			if (neighbours[n])
			{
				//Calculate the sum of the angles
				float sum = 0.0f;

				int neighbourPointIndex = -1;
				for (unsigned np = 0; np < 3; np++)
				{//For each of the neighbour's points
					if (neighbours[n]->points[np] == points[n])
					{//Common corner found
						neighbourPointIndex = np;
						break;
					}
				}
				if (neighbourPointIndex < 0)
					spehs::exceptions::unexpectedError("Triangle legalization failed!");

				//Determine begin and end arcs
				glm::vec2 begin;
				glm::vec2 end;
				//For common vertex 1
				begin = *neighbours[n]->point(neighbourPointIndex + 1) - *neighbours[n]->point(neighbourPointIndex);
				end = *point(n - 1) - *point(n);
				const float angle1 = spehs::getAngle(spehs::getAngle(begin), spehs::getAngle(end));
				//For common vertex 2
				begin = *point(n - 1) - *point(n + 1);
				end = *neighbours[n]->point(neighbourPointIndex + 1) - *neighbours[n]->point(neighbourPointIndex - 1);
				const float angle2 = spehs::getAngle(spehs::getAngle(begin), spehs::getAngle(end));
				////Conclusion
				if (angle1 + angle2 < PI)
				{//Exchange

					//std::cout << "\nEXCHANGE: " + std::to_string(n) + ", " + std::to_string(neighbourPointIndex);
					Triangle& other = *neighbours[n];

					//Exhange a single vertex
					other.point(neighbourPointIndex) = point(n - 1);
					this->point(n + 1) = other.point(neighbourPointIndex + 1);

					////Switch neighbour references
					bool found = false;
					Triangle* othersNewNeighbour = neighbour(n + 1);
					if (othersNewNeighbour)
					{
						for (unsigned n3 = 0; n3 < 3; n3++)
						{//For each neighbour in neighbour n + 1
							if (othersNewNeighbour->neighbours[n3] == this)
							{//Found this
								othersNewNeighbour->neighbours[n3] = &other;
								found = true;
								break;
							}
						}
						if (!found)
							spehs::exceptions::unexpectedError("Neighbour reference switching failed!");
					}
					other.neighbour(neighbourPointIndex - 1) = neighbour(n + 1);

					Triangle* myNewNeighbour = other.neighbour(neighbourPointIndex);
					found = false;
					if (myNewNeighbour)
					{
						for (unsigned n3 = 0; n3 < 3; n3++)
						{//For each neighbour in my new neighbour
							if (myNewNeighbour->neighbours[n3] == &other)
							{//Found other
								myNewNeighbour->neighbours[n3] = this;
								found = true;
								break;
							}
						}
						if (!found)
							spehs::exceptions::unexpectedError("Neighbour reference switching failed!");
					}
					neighbour(n) = myNewNeighbour;
					other.neighbour(neighbourPointIndex) = this;
					neighbour(n + 1) = &other;

					return false;
				}
				//else
				//	std::cout << "\nno exchange";
			}
		}
		for (unsigned i = 0; i < children.size(); i++)
		{
			if (!children[i]->legalize())
				return false;
		}
		return true;
	}
	TriangulationTest::TriangulationTest() : rng(/*spehs::rng::uirandom()*/triangulationTestSeed)
	{
		triangulationTestIteration++;
		points.resize(50);
		for (unsigned i = 0; i < points.size(); i++)
		{
			const float dir = rng.frandom(0.0f, TWO_PI);
			const float mag = rng.frandom(0.0f, std::min(applicationData->getWindowWidthHalf(), applicationData->getWindowHeightHalf()));
			const int animationLength = rng.sirandom(10, 100);
			const int animationRadius = rng.frandom(5.0f, 30.0f);
			const float animationState = float(triangulationTestIteration % animationLength) / float(animationLength);
			points[i] = new glm::vec2(
				applicationData->getWindowWidthHalf() + cos(dir) * mag + cos(TWO_PI * animationState) * animationRadius,
				applicationData->getWindowHeightHalf() + sin(dir) * mag + sin(TWO_PI * animationState) * animationRadius);
		}

		std::sort(points.begin(), points.end(), [](const glm::vec2* a, const glm::vec2* b) -> bool
		{
			return a->x < b->x;
		});

		////Add the first triangle
		//Check if on right left side
		if ((points[2]->x - points[0]->x) * (points[1]->y - points[0]->y) - (points[1]->x - points[0]->x) * (points[2]->y - points[0]->y) < 0.0f)
			triangles.push_back(new Triangle(points[0], points[1], points[2], nullptr));//point2 is on the left side of point0->point1
		else
			triangles.push_back(new Triangle(points[0], points[2], points[1], nullptr));//point2 is on the right side of point0->point1

		//Create hull linked list
		hull = new DirectionalEdge(triangles.back()->points[0], triangles.back()->points[1], nullptr);
		new DirectionalEdge(triangles.back()->points[1], triangles.back()->points[2], hull);
		new DirectionalEdge(triangles.back()->points[2], triangles.back()->points[0], hull->next);
		hull->innerTriangle = triangles.back();
		hull->next->innerTriangle = triangles.back();
		hull->next->next->innerTriangle = triangles.back();

		for (unsigned p = 3; p < points.size(); p++)
		{//For each point beyond the first triangle

			DirectionalEdge* edge = hull;
			while (edge)
			{//For each directional edge in the hull

				if ((points[p]->x - edge->begin->x) * (edge->end->y - edge->begin->y) - (edge->end->x - edge->begin->x) * (points[p]->y - edge->begin->y) > 0.0f)
				{//Is on the right side

					if (edge->prev && edge->prev->begin == points[p])
					{
						DirectionalEdge* high = new DirectionalEdge(points[p], edge->end, edge->prev->prev);
						high->setNext(edge->next);

						triangles.push_back(new Triangle(edge->begin, points[p], edge->end, edge->innerTriangle));
						bool found = false;
						for (unsigned i1 = 0; i1 < 3; i1++)
						{
							if (edge->innerTriangle->points[i1] == edge->begin)
							{
								edge->innerTriangle->neighbours[i1] = triangles.back();
								triangles.back()->neighbours[2] = edge->innerTriangle;
								found = true;
								break;
							}
						}
						if (!found)
							spehs::exceptions::unexpectedError("Could not assign inner triangle!");
						high->innerTriangle = triangles.back();
						//Link hull outline to triangle
						edge->prev->innerTriangle->neighbours[1] = triangles.back();
						triangles.back()->neighbours[0] = edge->prev->innerTriangle;

						if (edge == hull)
							hull = high->next;

						DirectionalEdge* remove = edge;
						edge = high->next;
						remove->next = nullptr;
						/*Removes prev*/
						delete remove;
					}
					else
					{
						DirectionalEdge* low = new DirectionalEdge(edge->begin, points[p], edge->prev);
						DirectionalEdge* high = new DirectionalEdge(points[p], edge->end, low);
						high->setNext(edge->next);

						triangles.push_back(new Triangle(edge->begin, points[p], edge->end, edge->innerTriangle));
						//Link inner edge triangle to newly created triangle
						bool found = false;
						for (unsigned i1 = 0; i1 < 3; i1++)
						{
							if (edge->innerTriangle->points[i1] == edge->begin)
							{
								edge->innerTriangle->neighbours[i1] = triangles.back();
								triangles.back()->neighbours[2] = edge->innerTriangle;
								found = true;
								break;
							}
						}
						if (!found)
							spehs::exceptions::unexpectedError("Could not assign inner triangle!");
						//Link hull outline to triangle
						low->innerTriangle = triangles.back();
						high->innerTriangle = triangles.back();

						if (edge == hull)
							hull = low;

						DirectionalEdge* remove = edge;
						edge = high->next;
						remove->prev = nullptr;
						remove->next = nullptr;
						delete remove;
					}
				}
				else
					edge = edge->next;
			}
		}

		//Legalization
		bool restart = false;
		do
		{
			if (triangles.front()->legalize())
				restart = false;
			else
				restart = true;
		} while (restart);


		std::cout << "\nTriangles: " + std::to_string(triangles.size());
	}
	TriangulationTest::~TriangulationTest()
	{
		for (unsigned i = 0; i < points.size(); i++)
			delete points[i];
		for (unsigned i = 0; i < triangles.size(); i++)
			delete triangles[i];
		if (hull)
		{
			delete hull;
		}
		for (unsigned i = 0; i < renderedPoints.size(); i++)
			renderedPoints[i]->destroy();
		if (triangleAllocations - triangleDeallocations != 0)
			std::cout << "\nTriangle leak: " + std::to_string(triangleAllocations - triangleDeallocations);
		if (edgeAllocations - edgeDeallocations != 0)
			std::cout << "\tEdge leak: " + std::to_string(edgeAllocations - edgeDeallocations);
		//std::getchar();
	}
	void TriangulationTest::update()
	{
		for (unsigned t = 0; t < triangles.size(); t++)
		{
			if (triangles[t]->polygon)
				triangles[t]->polygon->destroy();

			std::vector<glm::vec2> vertices(3);
			vertices[0] = *triangles[t]->points[0];
			vertices[1] = *triangles[t]->points[1];
			vertices[2] = *triangles[t]->points[2];
			triangles[t]->polygon = spehs::Polygon::create(vertices, 0, 1.0f, 1.0f);
			triangles[t]->polygon->setCameraMatrixState(false);
			triangles[t]->polygon->setColor(glm::vec4(rng.frandom(0.1f, 1.0f), rng.frandom(0.1f, 1.0f), rng.frandom(0.1f, 1.0f), 0.5f));

			if (false)
			{
				for (unsigned n = 0; n < 3; n++)
				{
					if (triangles[t]->neighbourArrows[n])
					{
						delete triangles[t]->neighbourArrows[n];
						triangles[t]->neighbourArrows[n] = nullptr;
					}
					if (triangles[t]->neighbours[n])
					{
						glm::vec2 myCenter(0.0f, 0.0f);
						glm::vec2 neighbourCenter(0.0f, 0.0f);
						for (unsigned c = 0; c < 3; c++)
						{
							myCenter += *triangles[t]->points[c] / 3.0f;
							neighbourCenter += *triangles[t]->neighbours[n]->points[c] / 3.0f;
						}

						triangles[t]->neighbourArrows[n] = new spehs::Arrow(myCenter, neighbourCenter);
						triangles[t]->neighbourArrows[n]->setCameraMatrixState(false);
						triangles[t]->neighbourArrows[n]->setLineThickness(0.7f);
						triangles[t]->neighbourArrows[n]->setArrowPointerSize(5.0f, 6.0f);
						triangles[t]->neighbourArrows[n]->setColor(255, 0, 128, 255);
					}
				}
			}

			if (triangles[t]->parentArrow)
			{
				delete triangles[t]->parentArrow;
				triangles[t]->parentArrow = nullptr;
			}
			if (triangles[t]->parent && false)
			{
				glm::vec2 myCenter(0.0f, 0.0f);
				glm::vec2 parentCenter(0.0f, 0.0f);
				for (unsigned c = 0; c < 3; c++)
				{
					myCenter += *triangles[t]->points[c] / 3.0f;
					parentCenter += *triangles[t]->parent->points[c] / 3.0f;
				}

				triangles[t]->parentArrow = new spehs::Arrow(myCenter, parentCenter);
				triangles[t]->parentArrow->setCameraMatrixState(false);
				triangles[t]->parentArrow->setLineThickness(1.7f);
				triangles[t]->parentArrow->setArrowPointerSize(10.0f, 13.0f);
				triangles[t]->parentArrow->setColor(128, 255, 0, 255);

			}
		}

		for (unsigned i = 0; i < renderedPoints.size(); i++)
			renderedPoints[i]->destroy();
		if (false)
		{
			renderedPoints.resize(points.size());
			for (unsigned i = 0; i < renderedPoints.size(); i++)
			{
				renderedPoints[i] = spehs::Point::create(1);
				renderedPoints[i]->setCameraMatrixState(false);
				renderedPoints[i]->setPosition(*points[i]);
			}
		}

		if (inputManager->isKeyDown(KEYBOARD_SPACE))
			triangulationTestSeed++;
	}
}