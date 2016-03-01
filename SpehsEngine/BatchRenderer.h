#pragma once

#include <vector>


namespace SpehsEngine
{
	class Primitive;
	class Polygon;
	class Line;
	class Point;
	class Mesh;
	class BatchObject;
	class Batch;
	class Camera2D;

	//Class that handles the management and rendering of batches
	class BatchRenderer
	{
		friend class Primitive;
		friend class Polygon;
		friend class Line;
		friend class Point;
		friend class Mesh;

	public:
		BatchRenderer(Camera2D* _camera);
		~BatchRenderer();

		void render();

	protected:
		Batch* addBatchObject(BatchObject* _primitive);

		std::vector<Batch*> batches;

		Camera2D* camera;
	};
	extern BatchRenderer* batchRenderer;
}
