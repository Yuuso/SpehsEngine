/*
Implementation by Teo Hiltunen, based on theory provided by: http://www.academicpub.org/jao/paperInfo.aspx?PaperID=15630
*/
#pragma once
#include <vector>
#include "SpehsEngine/Core/Vector.h"

namespace spehs
{
	struct Triangle;
	struct DirectionalEdge
	{
		DirectionalEdge(spehs::vec2* _begin, spehs::vec2* _end, DirectionalEdge* _prev);
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
		spehs::vec2* begin;
		spehs::vec2* end;
		DirectionalEdge* next;
		DirectionalEdge* prev;
		Triangle* innerTriangle;
	};
	struct Triangle
	{
		Triangle();
		~Triangle();
		void build(spehs::vec2* p0, spehs::vec2* p1, spehs::vec2* p2, Triangle* parent);
		bool legalize();
		Triangle*& neighbour(int index) { if (index < 0) return neighbours[3 + /*NOTE: works only for indices -1 and -2...*/(index % 3)]; return neighbours[index % 3]; }
		spehs::vec2*& point(int index) { if (index < 0) return points[3 + /*NOTE: works only for indices -1 and -2...*/(index % 3)]; return points[index % 3]; }
		int index3(int index) { if (index < 0) return 3 + /*NOTE: works only for indices -1 and -2...*/(index % 3); return index % 3; }
		
		spehs::vec2* points[3];
		Triangle* neighbours[3];
		/* Triangle hierarchy is used for legalization spreading purposes. The hierarchy does not imply any physical relation, but only as a data structure. */
		Triangle* parent;
		Triangle* children[3];//Unordered, may point to null. Only root node can have 3 children
	};

	/* Constructs triangles based on input points. points do not need to be ordered in any way. */
	class Triangulation
	{
	public:
		/* Performs triangulation on given points. */
		Triangulation(std::vector<spehs::vec2>& points);
		~Triangulation();

		Triangle& operator[](unsigned index){ return *triangles[index]; }
		size_t size() const{ return triangles.size(); }

	private:
		/* Points to the first element in the hull */
		DirectionalEdge* hull;
		/* Results of the triangulation algorithm are stored here. */
		std::vector<Triangle*> triangles;
	};
}