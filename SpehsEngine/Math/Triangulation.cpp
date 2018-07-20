#include "stdafx.h"
#include <algorithm>
#include <iostream>
#include <atomic>
#include "SpehsEngine/Math/Triangulation.h"
#include "SpehsEngine/Math/Geometry.h"
#define PERFORM_TRIANGULATION_VALIDITY_CHECKS

namespace se
{
	std::atomic<int> triangleAllocations(0);
	std::atomic<int> triangleDeallocations(0);
	std::atomic<int> edgeAllocations(0);
	std::atomic<int> edgeDeallocations(0);
	int triangulationTestSeed = 0;
	int triangulationTestIteration = 0;

	DirectionalEdge::DirectionalEdge(glm::vec2* _begin, glm::vec2* _end, DirectionalEdge* _prev) : begin(_begin), end(_end), prev(nullptr), next(nullptr), innerTriangle(nullptr)
	{
#ifdef _DEBUG
		edgeAllocations++;
#endif
		setPrev(_prev);
	}
	DirectionalEdge::~DirectionalEdge()
	{
#ifdef _DEBUG
		edgeDeallocations++;
#endif
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
	Triangle::Triangle()
	{
#ifdef _DEBUG
		triangleAllocations++;
#endif
		neighbours[0] = nullptr;
		neighbours[1] = nullptr;
		neighbours[2] = nullptr;
		parent = nullptr;
		children[0] = nullptr;
		children[1] = nullptr;
		children[2] = nullptr;
	}
	void Triangle::build(glm::vec2* p0, glm::vec2* p1, glm::vec2* p2, Triangle* _parent)
	{
		parent = _parent;
		if (parent)
		{
#ifdef PERFORM_TRIANGULATION_VALIDITY_CHECKS
			if (parent->children[0] != nullptr && parent->children[1] != nullptr && parent->children[2] != nullptr)
				log::error("Parent already has 3 children!");
#endif
			for (unsigned i = 0; i < 3; i++)
			{
				if (!parent->children[i])
				{
					parent->children[i] = this;
					break;
				}
			}
		}
		points[0] = p0;
		points[1] = p1;
		points[2] = p2;
	}
	Triangle::~Triangle()
	{
#ifdef _DEBUG
		triangleDeallocations++;
#endif
	}
	bool Triangle::legalize()
	{
#ifdef PERFORM_TRIANGULATION_VALIDITY_CHECKS
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
					log::error("Neighbours not double linked!");

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
					log::error("Neighbours do not have 2 common vertices!");

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
					log::error("Triangle contains 2 or more of the same vertex!");
			}
		}
#endif

		for (unsigned n = 0; n < 3; n++)
		{//For each neighbour

			if (neighbours[n])
			{
				int neighbourPointIndex = -1;
				for (unsigned np = 0; np < 3; np++)
				{//For each of the neighbour's points
					if (neighbours[n]->points[np] == points[n])
					{//Common corner found
						neighbourPointIndex = np;
						break;
					}
				}
#ifdef PERFORM_TRIANGULATION_VALIDITY_CHECKS
				if (neighbourPointIndex < 0)
					log::error("Triangle legalization failed!");
#endif

				//Determine begin and end arcs
				glm::vec2 begin;
				glm::vec2 end;
				//For common vertex 1
				begin = *neighbours[n]->point(neighbourPointIndex + 1) - *neighbours[n]->point(neighbourPointIndex);
				end = *point(n - 1) - *point(n);
				const float angle1 = se::getAngle(se::getAngle(begin), se::getAngle(end));
				//For common vertex 2
				begin = *point(n - 1) - *point(n + 1);
				end = *neighbours[n]->point(neighbourPointIndex + 1) - *neighbours[n]->point(neighbourPointIndex - 1);
				const float angle2 = se::getAngle(se::getAngle(begin), se::getAngle(end));
				////Conclusion
				if (angle1 + angle2 < PI)
				{//Exchange

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
#ifdef PERFORM_TRIANGULATION_VALIDITY_CHECKS
						if (!found)
							log::error("Neighbour reference switching failed!");
#endif
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
#ifdef PERFORM_TRIANGULATION_VALIDITY_CHECKS
						if (!found)
							log::error("Neighbour reference switching failed!");
#endif
					}
					neighbour(n) = myNewNeighbour;
					other.neighbour(neighbourPointIndex) = this;
					neighbour(n + 1) = &other;

					return false;
				}
			}
		}
		for (unsigned i = 0; i < 3; i++)
		{
			if (children[i])
			{//Valid to spread legalization
				if (!children[i]->legalize())
					return false;
			}
		}
		return true;
	}
	Triangulation::Triangulation(std::vector<glm::vec2>& points)
	{
		if (points.size() < 3)
			log::error("Triangulation must have at least 3 input points!");

		std::sort(points.begin(), points.end(), [](const glm::vec2 a, const glm::vec2 b) -> bool
		{
			return a.x < b.x;
		});

		////Add the first triangle
		triangles.push_back(new Triangle());
		//Check if on right left side
		if ((points[2].x - points[0].x) * (points[1].y - points[0].y) - (points[1].x - points[0].x) * (points[2].y - points[0].y) < 0.0f)
			triangles.back()->build(&points[0], &points[1], &points[2], nullptr);//point2 is on the left side of point0->point1
		else
			triangles.back()->build(&points[0], &points[2], &points[1], nullptr);//point2 is on the right side of point0->point1

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

				if ((points[p].x - edge->begin->x) * (edge->end->y - edge->begin->y) - (edge->end->x - edge->begin->x) * (points[p].y - edge->begin->y) > 0.0f)
				{//Is on the right side

					if (edge->prev && edge->prev->begin == &points[p])
					{
						DirectionalEdge* high = new DirectionalEdge(&points[p], edge->end, edge->prev->prev);
						high->setNext(edge->next);

						triangles.push_back(new Triangle());
						triangles.back()->build(edge->begin, &points[p], edge->end, edge->innerTriangle);
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
#ifdef PERFORM_TRIANGULATION_VALIDITY_CHECKS
						if (!found)
							log::error("Could not assign inner triangle!");
#endif

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
						DirectionalEdge* low = new DirectionalEdge(edge->begin, &points[p], edge->prev);
						DirectionalEdge* high = new DirectionalEdge(&points[p], edge->end, low);
						high->setNext(edge->next);

						triangles.push_back(new Triangle());
						triangles.back()->build(edge->begin, &points[p], edge->end, edge->innerTriangle);
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
							log::error("Could not assign inner triangle!");
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
	}
	Triangulation::~Triangulation()
	{
		if (hull)
			delete hull;
		for (unsigned i = 0; i < triangles.size(); i++)
			delete triangles[i];
	}

}