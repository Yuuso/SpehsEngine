/*
	Example use intended to demonstrate a sweep-line triangulation algorithm.
	The example randomizes a group of points and triangulates a convex form in between them,
	colouring each triangle randomly while animating at the same time.
		
	NOTE: For the animation to work the class must be deallocated and reconstructed each update cycle.
*/
#pragma once
#include "Exceptions.h"
#include "RNG.h"
#include <glm/vec2.hpp>
#include <vector>
#include <string>
#include <atomic>
namespace spehs
{
	class Polygon;
	class Arrow;
	class Point;


	struct Triangle;
	struct DirectionalEdge
	{
		DirectionalEdge(glm::vec2* _begin, glm::vec2* _end, DirectionalEdge* _prev);
		~DirectionalEdge();

		void setPrev(DirectionalEdge* _prev)
		{
			if (prev)
				prev->next = nullptr;
			prev = _prev;
			if (prev)
				prev->next = this;
		}
		void setNext(DirectionalEdge* _next)
		{
			if (next)
				next->prev = nullptr;
			next = _next;
			if (next)
				next->prev = this;
		}
		glm::vec2* begin;
		glm::vec2* end;
		DirectionalEdge* next;
		DirectionalEdge* prev;
		Triangle* innerTriangle;
	};
	struct Triangle
	{
		Triangle(glm::vec2* p0, glm::vec2* p1, glm::vec2* p2, Triangle* parent);
		~Triangle();
		bool legalize();
		glm::vec2* points[3];
		Triangle* neighbours[3];
		Triangle*& neighbour(int index) { if (index < 0) return neighbours[3 + /*NOTE: works only for indices -1 and -2...*/(index % 3)]; return neighbours[index % 3]; }
		glm::vec2*& point(int index) { if (index < 0) return points[3 + /*NOTE: works only for indices -1 and -2...*/(index % 3)]; return points[index % 3]; }
		int index3(int index) { if (index < 0) return 3 + /*NOTE: works only for indices -1 and -2...*/(index % 3); return index % 3; }
		//-1>2 -2>1 -3>0
		//Hierarchy
		Triangle* parent;
		std::vector<Triangle*> children;

		//DEBUG
		spehs::Polygon* polygon;
		spehs::Arrow* neighbourArrows[3];
		spehs::Arrow* parentArrow;
	};
	class TriangulationTest
	{
	public:
		TriangulationTest();
		~TriangulationTest();

		void update();

		std::vector<glm::vec2*> points;
		std::vector<spehs::Point*> renderedPoints;
		std::vector<Triangle*> triangles;
		DirectionalEdge* hull;//Points to the first element in the hull
		spehs::rng::PRNG32 rng;
	};
}